#ifndef WEB_PAGES_H
#define WEB_PAGES_H

#include <Arduino.h>

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>ESP32 ADC & DAC Laboratory</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; text-align: center; margin: 0; padding: 20px; background-color: #f4f7f6; color: #333; }
        .card { background: white; padding: 20px; margin: 15px auto; max-width: 450px; border-radius: 10px; box-shadow: 0 4px 10px rgba(0,0,0,0.08); }
        h2 { margin-top: 0; color: #0056b3; }
        .metric { font-size: 1.1em; margin: 8px 0; }
        canvas { background: #1e1e1e; border-radius: 6px; margin-top: 10px; }
        button { padding: 12px 24px; font-size: 15px; font-weight: bold; background-color: #007bff; color: white; border: none; border-radius: 6px; cursor: pointer; transition: background 0.2s; }
        button:hover { background-color: #0056b3; }
    </style>
</head>
<body>
    <div class="card">
        <h2>ESP32 ADC / DAC Monitor</h2>
        <div class="metric">ADC Raw (12-bit): <b><span id="adc_val">0</span></b></div>
        <div class="metric">DAC Output (8-bit): <b><span id="dac_val">0</span></b> (<span id="volt_val">0.00</span> V)</div>
        <div class="metric">HW Button: <b><span id="hw_btn">OFF</span></b> | Web Button: <b><span id="web_btn">OFF</span></b></div>
        <br>
        <button onclick="toggleWebButton()">Toggle Web State</button>
    </div>
    <div class="card">
        <canvas id="realtimeGraph" width="380" height="160"></canvas>
    </div>

    <script>
        const canvas = document.getElementById('realtimeGraph');
        const ctx = canvas.getContext('2d');
        const bufferSize = 50;
        let dataPoints = new Array(bufferSize).fill(0);

        function renderGraph() {
            ctx.clearRect(0, 0, canvas.width, canvas.height);
            ctx.beginPath();
            ctx.moveTo(0, canvas.height - (dataPoints[0] / 4095 * canvas.height));
            for (let i = 1; i < dataPoints.length; i++) {
                let x = i * (canvas.width / (bufferSize - 1));
                let y = canvas.height - (dataPoints[i] / 4095 * canvas.height);
                ctx.lineTo(x, y);
            }
            ctx.strokeStyle = '#00e5ff';
            ctx.lineWidth = 2;
            ctx.stroke();
        }

        function fetchTelemetryData() {
            fetch('/api/data')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('adc_val').innerText = data.adc;
                    document.getElementById('dac_val').innerText = data.dac;
                    
                    let voltage = (data.dac / 255.0) * 3.3;
                    document.getElementById('volt_val').innerText = voltage.toFixed(2);
                    
                    document.getElementById('hw_btn').innerText = data.btn_hw ? "ON" : "OFF";
                    document.getElementById('web_btn').innerText = data.btn_web ? "ON" : "OFF";
                    
                    dataPoints.push(data.adc);
                    dataPoints.shift();
                    renderGraph();
                })
                .catch(err => console.error("Telemetry fetch error:", err));
        }

        function toggleWebButton() {
            fetch('/api/toggle', { method: 'POST' });
        }

        setInterval(fetchTelemetryData, 300);
    </script>
</body>
</html>
)rawliteral";

#endif