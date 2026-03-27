// Prevents additional console window on Windows in release, DO NOT REMOVE!!
#![cfg_attr(not(debug_assertions), windows_subsystem = "windows")]

fn main() {
<<<<<<< Updated upstream
    tauri::Builder::default();
        .plugin(tauri_plugin_serialplugin::init())
        .run(tauri::generate_context!())
        .expect("Error while running tauri application")
=======
    tauri::Builder::default()
        .plugin(tauri_plugin_serialplugin::init())
        .run(tauri::generate_context!())
        .expect("error while running tauri application");
>>>>>>> Stashed changes
    swanganzapp_lib::run()
}
