mod state;
use std::fs::OpenOptions;
use std::io::BufWriter;
use std::io::Write;
use std::sync::Mutex;

use tauri::{AppHandle, Emitter, State};
use tauri_plugin_serialplugin::commands::{
    available_ports, close, managed_ports, open, read, read_binary, write,
};
use tauri_plugin_serialplugin::state::{DataBits, FlowControl, Parity, StopBits};

use crate::state::SensorQueues;

pub struct ConnectedPort(pub Mutex<Option<String>>);

#[cfg_attr(mobile, tauri::mobile_entry_point)]
pub fn run() {
    tauri::Builder::default()
        .plugin(tauri_plugin_serialplugin::init())
        .plugin(tauri_plugin_opener::init())
        .plugin(tauri_plugin_log::Builder::new().build())
        .manage(SensorQueues::new())
        .manage(ConnectedPort(Mutex::new(None)))
        .invoke_handler(tauri::generate_handler![
            check_connected,
            check_disconnected,
            get_data,
            drain_queues,
            export_csv
        ])
        .run(tauri::generate_context!())
        .expect("error while running tauri application");
}

// Checks available ports and opens port if found. Emits "port-connected"
#[tauri::command]
async fn check_connected(
    app: AppHandle<tauri::Wry>,
    serial: State<'_, tauri_plugin_serialplugin::desktop_api::SerialPort<tauri::Wry>>,
    connected_port: State<'_, ConnectedPort>,
) -> Result<(), String> {
    let ports = available_ports(app.clone(), serial.clone()).map_err(|e| e.to_string())?;

    for (port_name, _port_info) in ports.iter() {
        app.emit("available_ports", port_name).unwrap();
        if port_name.contains("cu.usbserial-BG04P2N") {
            open(
                app.clone(),
                serial.clone(),
                port_name.to_string(),
                128000,
                Some(DataBits::Eight),
                Some(FlowControl::None),
                Some(Parity::None),
                None,          // StopBits
                Some(1000u64), // Timeout
            )
            .map_err(|e| format!("Failed to open port: {}", e))?;

            *connected_port.0.lock().unwrap() = Some(port_name.to_string());
            app.emit("port-connected", port_name).unwrap();
        }
    }

    Ok(())
}

// Checks managed ports and emits "port-disconnected" if empty
#[tauri::command]
async fn check_disconnected(
    app: AppHandle<tauri::Wry>,
    serial: State<'_, tauri_plugin_serialplugin::desktop_api::SerialPort<tauri::Wry>>,
) -> Result<(), String> {
    let available = available_ports(app.clone(), serial.clone()).map_err(|e| e.to_string())?;
    let mut still_connected = false;
    let mut port = String::new();
    for (port_name, _port_info) in available.iter() {
        if port_name.contains("cu.usbserial-BG04P2N") {
            still_connected = true;
            port = port_name.to_string();
        }
    }
    if !still_connected {
        let managed = managed_ports(app.clone(), serial.clone()).map_err(|e| e.to_string())?;
        for port_name in managed.iter() {
            if port_name.contains("cu.usbserial-BG04P2N") {
                close(app.clone(), serial.clone(), port_name.to_string())
                    .map_err(|e| e.to_string())?;
            }
        }
        app.emit("port-disconnected", "cu.usbserial-BG04P2N")
            .unwrap();
    }

    Ok(())
}

// Reads data from UART and adds it to dataset
#[tauri::command]
async fn get_data(
    app: AppHandle<tauri::Wry>,
    serial: State<'_, tauri_plugin_serialplugin::desktop_api::SerialPort<tauri::Wry>>,
    queues: State<'_, SensorQueues>,
    connected_port: State<'_, ConnectedPort>,
) -> Result<(), String> {
    let path = connected_port
        .0
        .lock()
        .unwrap()
        .clone()
        .ok_or("No port connected")?;

    let data_ascii = [0xFA];

    let mut header = match read_binary(
        app.clone(),
        serial.clone(),
        path.clone(),
        Some(1000u64),
        Some(1usize),
    ) {
        Ok(data) => data,
        Err(_) => return Ok(()),
    };

    if header != data_ascii {
        return Ok(());
    }

    app.emit("data-begin", &path).unwrap();
    'outer: while header == data_ascii {
        let received_data = match read_binary(
            app.clone(),
            serial.clone(),
            path.clone(),
            Some(1000u64),
            Some(6usize),
        ) {
            Ok(data) => data,
            Err(_) => break,
        };

        let p1 = (received_data[0] as u16) << 8 | received_data[1] as u16;
        let p2 = (received_data[2] as u16) << 8 | received_data[3] as u16;
        let temp = (received_data[4] as u16) << 8 | received_data[5] as u16;

        queues.p1.lock().unwrap().push_back(p1);
        queues.p2.lock().unwrap().push_back(p2);
        queues.temp.lock().unwrap().push_back(temp);

        let max_search = 12; // give up after this many bytes searched
        for _ in 0..max_search {
            header = match read_binary(
                app.clone(),
                serial.clone(),
                path.clone(),
                Some(1000u64),
                Some(1usize),
            ) {
                Ok(data) => data,
                Err(_) => break 'outer,
            };
            if header == data_ascii {
                continue 'outer;
            }
        }
        break;
    }

    app.emit("data-done", &path).unwrap();
    Ok(())
}

#[tauri::command]
fn drain_queues(queues: State<'_, SensorQueues>) -> (Vec<u16>, Vec<u16>, Vec<u16>) {
    let p1 = queues.p1.lock().unwrap().drain(..).collect();
    let p2 = queues.p2.lock().unwrap().drain(..).collect();
    let temp = queues.temp.lock().unwrap().drain(..).collect();
    (p1, p2, temp)
}

#[tauri::command]
fn export_csv(data: Vec<(u16, u16, u16)>, path: String) -> Result<(), String> {
    let file = OpenOptions::new()
        .write(true)
        .create(true)
        .truncate(true)
        .open(&path)
        .map_err(|e| e.to_string())?;

    let mut writer = BufWriter::new(file);
    writeln!(writer, "pressure1,pressure2,temperature").map_err(|e| e.to_string())?;
    for (p1, p2, temp) in &data {
        writeln!(writer, "{},{},{}", p1, p2, temp).map_err(|e| e.to_string())?;
    }

    writer.flush().map_err(|e| e.to_string())?;

    Ok(())
}
