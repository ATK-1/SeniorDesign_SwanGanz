import { listen } from "@tauri-apps/api/event";
import { invoke } from '@tauri-apps/api/core';

let pollInterval;
let connected = false;

await listen("port-connected", (event) => {
    const element = document.getElementsByClassName("connection-status")[0];
    element.textContent = "Connected";
    console.log("Connected");
    connected = true;
});

await listen("port-disconnected", (event) => {
    const element = document.getElementsByClassName("connection-status")[0];
    element.textContent = "Not Connected";
    console.log("Disconnected");
    connected = false;
});



pollInterval = setInterval(async () => {
    if (!connected) {
        try {
            await invoke("check_connected");
        }
        catch (e) {
            console.error("Error checking ports:", e);
        }
    }
    else {
        try {
            await invoke("check_disconnected");
        }
        catch (e) {
            console.error("Error checking ports:", e);
        }
    }
}, 1500);

