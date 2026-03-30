use tauri::{AppHandle, Emitter, State};
use tauri_plugin_serialplugin::commands::{available_ports, close, open, read, write};

#[tauri::command]
fn ping() -> String {
    "pong".to_string()
}

#[cfg_attr(mobile, tauri::mobile_entry_point)]
pub fn run() {
    tauri::Builder::default()
        .plugin(tauri_plugin_serialplugin::init())
        .plugin(tauri_plugin_opener::init())
        .plugin(tauri_plugin_log::Builder::new().build())
        .invoke_handler(tauri::generate_handler![check_connected, ping])
        .run(tauri::generate_context!())
        .expect("error while running tauri application");
}

#[tauri::command]
async fn check_connected(
    app: AppHandle<tauri::Wry>,
    serial: State<'_, tauri_plugin_serialplugin::desktop_api::SerialPort<tauri::Wry>>,
) -> Result<(), String> {
    let ports = available_ports(app.clone(), serial.clone()).map_err(|e| e.to_string())?;

    for (port_name, _port_info) in ports.iter() {
        if port_name.contains("cu.usbserial-A106DAXQ") {
            app.emit("port-connected", port_name).unwrap();
        }
    }

    Ok(())
}
