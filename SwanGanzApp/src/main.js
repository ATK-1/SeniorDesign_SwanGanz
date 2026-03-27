import { SerialPort } from "tauri-plugin-serialplugin";
const { invoke } = window.__TAURI__.core;


document.addEventListener("DOMContentLoaded", (event) => {
    console.log("DOM fully loaded and parsed");
    const ports = await SerialPort.available_ports();
    console.log(ports);
});
