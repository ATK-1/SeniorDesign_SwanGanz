import { listen } from "@tauri-apps/api/event";
import { invoke } from '@tauri-apps/api/core';
import Chart from "chart.js/auto";

let connected = false;
let dataInterval = false;
let drainInterval = false;

await listen("port-connected", (event) => {
    console.log(event);
    const statusElement = document.getElementsByClassName("connection-status")[0];
    const circleElement = document.getElementsByClassName("connection-circle")[0];
    statusElement.textContent = "Connected";
    circleElement.style["background-color"] = "green";
    console.log("Connected");
    connected = true;
    dataInterval = setInterval(async () => {
        if (connected) {
            try {
                await invoke("get_data");
            }
            catch (e) {
                console.error("Error getting data:", e);
            }
        }
    }, 200);
});

await listen("port-disconnected", (event) => {
    const statusElement = document.getElementsByClassName("connection-status")[0];
    const circleElement = document.getElementsByClassName("connection-circle")[0];
    statusElement.textContent = "Not Connected";
    circleElement.style["background-color"] = "#bbb";
    connected = false;

    if (drainInterval) {
        clearInterval(drainInterval);
        drainInterval = null;
    }
    if (dataInterval) {
        clearInterval(dataInterval);
        dataInterval = null;
    }
});

await listen("data-begin", (event) => {
    console.log("data begin");
    if (dataInterval) {
        clearInterval(dataInterval);
        dataInterval = null;
    }
    if (!drainInterval) {
        drainInterval = setInterval(async () => {
            const [p1, p2, temp] = await invoke("drain_queues");
            if (p1.length > 0) {
                updateGraph(p1, p2, temp);
            }
        }, 10);
    }
});

await listen("data-done", (event) => {
    console.log("data done");
    if (drainInterval) {
        clearInterval(drainInterval);
        drainInterval = null;
    }
});


setInterval(async () => {
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


const pressureCanvas = document.getElementById("pressureGraph");
const pressureGraph = new Chart(pressureCanvas, {
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

const tempCanvas = document.getElementById("tempGraph");
const tempGraph = new Chart(tempCanvas, {
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

function updateGraph(p1Vals, p2Vals, tempVals) {
    pressureGraph.data.datasets[0].data.push(...p1Vals);
    pressureGraph.data.datasets[1].data.push(...p2Vals);
    tempGraph.data.datasets[0].data.push(...tempVals);
    pressureGraph.update();
    tempGraph.update();
}
