use tauri::{AppHandle, Emitter, State};
use tauri_plugin_serialplugin::commands::{
    available_ports, close, managed_ports, open, read, write,
};

#[cfg_attr(mobile, tauri::mobile_entry_point)]
pub fn run() {
    tauri::Builder::default()
        .plugin(tauri_plugin_serialplugin::init())
        .plugin(tauri_plugin_opener::init())
        .plugin(tauri_plugin_log::Builder::new().build())
        .invoke_handler(tauri::generate_handler![
            check_connected,
            check_disconnected
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
                9600,
                None,
                None,
                None,
                None,
                None,
            );
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
