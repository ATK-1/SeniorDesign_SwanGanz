import { listen } from "@tauri-apps/api/event";
import { invoke } from '@tauri-apps/api/core';
import Chart from "chart.js/auto";

let connected = false;
let dataInterval = false;
let gettingData = false;
let drainInterval = false;


await listen("debug-header", (event) => {
    console.log("header byte:", event.payload.toString(16));
});

await listen("port-connected", (event) => {
    console.log(event);
    const statusElement = document.getElementsByClassName("connection-status")[0];
    const circleElement = document.getElementsByClassName("connection-circle")[0];
    statusElement.textContent = "Connected";
    circleElement.style["background-color"] = "green";
    console.log("Connected");
    connected = true;
    dataInterval = setInterval(async () => {
        if (connected && !gettingData) {
            gettingData = true;
            try {
                await invoke("get_data");
            }
            catch (e) {
                console.error("Error getting data:", e);
            }
            finally {
                gettingData = false;
            }
        }
    }, 200);
});

//await listen("available_ports", (event) => {
//    console.log(event);
//});

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
    if (dataInterval) {
        clearInterval(dataInterval);
        dataInterval = null;
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
    },
    options: {
        responsive: true,
        maintainAspectRatio: false,
        animation: false,
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
    },
    options: {
        responsive: true,
        maintainAspectRatio: false,
        animation: false,
    }
});

let index = 0;
function updateGraph(p1Vals, p2Vals, tempVals) {
    const p1Points = p1Vals.map((y, i) => ({ x: index + i, y }));
    const p2Points = p2Vals.map((y, i) => ({ x: index + i, y }));
    const tempPoints = tempVals.map((y, i) => ({ x: index + i, y }));

    index += p1Vals.length;

    pressureGraph.data.datasets[0].data.push(...p1Points);
    pressureGraph.data.datasets[1].data.push(...p2Points);
    tempGraph.data.datasets[0].data.push(...tempPoints);
    pressureGraph.update();
    tempGraph.update();
}

const button = document.getElementById("csv-button");
button.addEventListener("click", function() {
    console.log("Button clicked");
    const p1Data = pressureGraph.data.datasets[0].data;
    const p2Data = pressureGraph.data.datasets[1].data;
    const tempData = tempGraph.data.datasets[0].data;

    const rows = p1Data.map((_, i) => [p1Data[i].y, p2Data[i].y, tempData[i].y]);
    const fileInput = document.getElementById("file-name-input");
    let fileName = fileInput.value;

    if (!fileName.endsWith(".txt")) {
        fileName += ".txt"
    }

    invoke("export_csv", {
        data: rows,
        path: fileName,
    });

    const container = document.querySelector(".file-create-container");
    container.innerHTML = `<h3>${fileName} created</h3>`;
});

const resetButton = document.getElementById("reset-button");
resetButton.addEventListener("click", function() {
    pressureGraph.data.datasets[0].data = [];
    pressureGraph.data.datasets[1].data = [];
    tempGraph.data.datasets[0].data = [];
    pressureGraph.update();
    tempGraph.update();
    index = 0;

    const container = document.querySelector(".file-create-container");
    container.innerHTML = `
        <input type="text" class="file-create-button" id="file-name-input" value="data.txt">
        <button class="file-create-button" id="csv-button">Save as CSV</button>
        `;

    document.getElementById("csv-button").addEventListener("click", csvButtonHandler);
});
