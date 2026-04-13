mod state;
use tauri::{AppHandle, Emitter, State};
use tauri_plugin_serialplugin::commands::{
    available_ports, close, managed_ports, open, read, read_binary, write,
};
use tauri_plugin_serialplugin::state::{DataBits, FlowControl, Parity, StopBits};

use crate::state::SensorQueues;

#[cfg_attr(mobile, tauri::mobile_entry_point)]
pub fn run() {
    tauri::Builder::default()
        .plugin(tauri_plugin_serialplugin::init())
        .plugin(tauri_plugin_opener::init())
        .plugin(tauri_plugin_log::Builder::new().build())
        .manage(SensorQueues::new())
        .invoke_handler(tauri::generate_handler![
            check_connected,
            check_disconnected,
            get_data,
            drain_queues
        ])
        .run(tauri::generate_context!())
        .expect("error while running tauri application");
}

// Checks available ports and opens port if found. Emits "port-connected"
#[tauri::command]
async fn check_connected(
    app: AppHandle<tauri::Wry>,
    serial: State<'_, tauri_plugin_serialplugin::desktop_api::SerialPort<tauri::Wry>>,
) -> Result<(), String> {
    let ports = available_ports(app.clone(), serial.clone()).map_err(|e| e.to_string())?;

    for (port_name, _port_info) in ports.iter() {
        if port_name.contains("cu.usbserial-A106DAXQ") {
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
        if port_name.contains("cu.usbserial-A106DAXQ") {
            still_connected = true;
            port = port_name.to_string();
        }
    }
    if !still_connected {
        let managed = managed_ports(app.clone(), serial.clone()).map_err(|e| e.to_string())?;
        for port_name in managed.iter() {
            if port_name.contains("cu.usbserial-A106DAXQ") {
                close(app.clone(), serial.clone(), port_name.to_string())
                    .map_err(|e| e.to_string())?;
            }
        }
        app.emit("port-disconnected", "cu.usbserial-A106DAXQ")
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
) -> Result<(), String> {
    let path = "/dev/cu.usbserial-A106DAXQ".to_string();
    let data_ascii = [0x44, 0x41, 0x54, 0x41];
    let mut header = read_binary(
        app.clone(),
        serial.clone(),
        path.clone(),
        Some(1000u64),
        Some(4usize),
    )
    .map_err(|e| format!("Failed to read binary data: {}", e))?;

    if header == data_ascii {
        app.emit("data-begin", &path).unwrap();
    }
    while header == data_ascii {
        let received_data = read_binary(
            app.clone(),
            serial.clone(),
            path.clone(),
            Some(1000u64),
            Some(6usize),
        )
        .map_err(|e| format!("Failed to read binary data: {}", e))?;

        queues
            .p1
            .lock()
            .unwrap()
            .push_back((received_data[0] as u16) << 8 | received_data[1] as u16);
        queues
            .p2
            .lock()
            .unwrap()
            .push_back((received_data[2] as u16) << 8 | received_data[3] as u16);
        queues
            .temp
            .lock()
            .unwrap()
            .push_back((received_data[4] as u16) << 8 | received_data[5] as u16);

        header = read_binary(
            app.clone(),
            serial.clone(),
            path.clone(),
            Some(1000u64),
            Some(4usize),
        )
        .map_err(|e| format!("Failed to read binary data: {}", e))?;
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
