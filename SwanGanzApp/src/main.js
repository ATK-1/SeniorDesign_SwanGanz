import { listen } from "@tauri-apps/api/event";
import { invoke } from '@tauri-apps/api/core';
import Chart from "chart.js/auto";

let pollInterval;
let connected = false;

await listen("port-connected", (event) => {
    const statusElement = document.getElementsByClassName("connection-status")[0];
    const circleElement = document.getElementsByClassName("connection-circle")[0];
    statusElement.textContent = "Connected";
    circleElement.style["background-color"] = "green";
    console.log("Connected");
    connected = true;
});

await listen("port-disconnected", (event) => {
    const statusElement = document.getElementsByClassName("connection-status")[0];
    const circleElement = document.getElementsByClassName("connection-circle")[0];
    statusElement.textContent = "Not Connected";
    circleElement.style["background-color"] = "#bbb";
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
}, 1000);


const pressureGraph = document.getElementById("pressureGraph");
new Chart(pressureGraph, {
    type: "scatter",
    data: {
        labels: [],
        datasets: [
            {
                label: "Pressure 1",
                data: [],
            },
            {
                label: "Pressure 2",
                data: []
            }
        ]
    }
});

const tempGraph = document.getElementById("tempGraph");
new Chart(tempGraph, {
    type: "scatter",
    data: {
        labels: [],
        datasets: [
            {
                label: "Temperature",
                data: [],
            }
        ]
    }
});
