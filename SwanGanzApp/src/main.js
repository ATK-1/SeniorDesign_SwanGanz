import { listen } from "@tauri-apps/api/event";
import { invoke } from '@tauri-apps/api/core';

let pollInterval;

try {
    invoke("ping")
} catch (e) {
    console.error("ping error:", e);
}

await listen("port-connected", (event) => {
    console.log("Port found: ", event.payload);
    clearInterval(pollInterval);
    // transition UI state here
});

pollInterval = setInterval(async () => {
    try {
        await invoke("check_connected");
    } catch (e) {
        console.error("Error checking ports:", e);
    }
}, 1500);
