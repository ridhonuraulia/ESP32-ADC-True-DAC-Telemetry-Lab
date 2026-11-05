
#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const char* ssid = "NAME_WIFI";
const char* password = "PASSWORD_WIFI";

// 0.96-inch SSD1306 OLED Configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

WebServer server(80);

// Pin Definitions
const int PIN_POT     = 34; // ADC1 Pin (PV Input)
const int PIN_DAC     = 25; // True DAC1 Output Pin
const int PIN_BTN_HW  = 14;

// Control and Sensor Parameters
int pvValue = 0;           // Process Variable (0 - 4095)
int setpointValue = 2048;  // Default Set Point (12-bit midpoint)
int errorValue = 0;        // Error = SP - PV
int dacValue = 0;          // DAC Output (8-bit: 0 - 255)

bool stateBtnHW = false;
bool stateBtnWeb = false;

// HTML and Client-side Script with Setpoint Control
// and Dual Graph (SP vs PV)
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>ESP32 Control Lab: SP, PV & Error</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body { font-family: Arial, sans-serif; text-align: center; margin: 0; padding: 15px; background-color: #f4f4f4;}
        .card { background: white; padding: 15px; margin: 10px auto; max-width: 420px; border-radius: 8px; box-shadow: 0 2px 5px rgba(0,0,0,0.1); }
        .val-box { font-size: 1.1em; font-weight: bold; }
        .err-pos { color: #d9534f; }
        .err-neg { color: #0275d8; }
        canvas { background: #111; border-radius: 4px; }
        button { padding: 10px 20px; font-size: 14px; background-color: #28a745; color: white; border: none; border-radius: 4px; cursor: pointer; }
        input[type=range] { width: 80%; margin: 10px 0; }
    </style>
</head>
<body>
    <div class="card">
        <h2>ESP32 SP / PV / Error Monitor</h2>

        <p>Set Point (SP): <b><span id="sp_val">2048</span></b></p>
        <input type="range" id="sp_slider" min="0" max="4095" value="2048"
        onchange="sendSetpoint(this.value)"
        oninput="document.getElementById('sp_val').innerText = this.value">

        <hr>

        <p>Process Variable (PV): <b><span id="pv_val">0</span></b>
        (<span id="pv_volt">0.00</span> V)</p>

        <p>Error (SP - PV):
        <span id="err_box" class="val-box">
        <span id="err_val">0</span></span>
        (<span id="err_volt">0.00</span> V)</p>

        <p>DAC Output (8-bit): <b><span id="dac_val">0</span></b></p>

        <hr>

        <p>Hardware Button: <span id="hw_btn">OFF</span>
        | Web Button: <span id="web_btn">OFF</span></p>

        <button onclick="toggleWebBtn()">Toggle Web Button</button>
    </div>

    <div class="card">
        <p style="margin: 5px; font-size: 12px; text-align: left;">
            <span style="color:#00e5ff;">&#9644; PV (Sensor)</span> &nbsp;&nbsp;
            <span style="color:#ff4444;">&#9644; Set Point</span>
        </p>

        <canvas id="graph" width="380" height="160"></canvas>
    </div>

    <script>
        var canvas = document.getElementById('graph');
        var ctx = canvas.getContext('2d');

        var pvHistory = new Array(40).fill(0);
        var spHistory = new Array(40).fill(2048);

        function drawGraph() {
            ctx.clearRect(0, 0, canvas.width, canvas.height);

            // Draw Grid Lines
            ctx.strokeStyle = '#333';
            ctx.lineWidth = 1;
            ctx.beginPath();
            ctx.moveTo(0, canvas.height/2);
            ctx.lineTo(canvas.width, canvas.height/2);
            ctx.stroke();

            // Plot SP (Red Line)
            ctx.beginPath();
            ctx.moveTo(0, canvas.height - (spHistory[0] / 4095 * canvas.height));

            for (var i = 1; i < spHistory.length; i++) {
                var x = i * (canvas.width / (spHistory.length - 1));
                var y = canvas.height - (spHistory[i] / 4095 * canvas.height);
                ctx.lineTo(x, y);
            }

            ctx.strokeStyle = '#ff4444';
            ctx.lineWidth = 2;
            ctx.stroke();

            // Plot PV (Cyan Line)
            ctx.beginPath();
            ctx.moveTo(0, canvas.height - (pvHistory[0] / 4095 * canvas.height));

            for (var i = 1; i < pvHistory.length; i++) {
                var x = i * (canvas.width / (pvHistory.length - 1));
                var y = canvas.height - (pvHistory[i] / 4095 * canvas.height);
                ctx.lineTo(x, y);
            }

            ctx.strokeStyle = '#00e5ff';
            ctx.lineWidth = 2;
            ctx.stroke();
        }

        function fetchData() {
            fetch('/data').then(res => res.json()).then(data => {
                document.getElementById('pv_val').innerText = data.pv;
                document.getElementById('dac_val').innerText = data.dac;
                document.getElementById('err_val').innerText = data.error;
                document.getElementById('sp_val').innerText = data.sp;
                document.getElementById('sp_slider').value = data.sp;

                var pvVolt = (data.pv / 4095.0) * 3.3;
                var errVolt = (data.error / 4095.0) * 3.3;

                document.getElementById('pv_volt').innerText = pvVolt.toFixed(2);
                document.getElementById('err_volt').innerText = errVolt.toFixed(2);

                document.getElementById('hw_btn').innerText = data.btn_hw ? "ON" : "OFF";
                document.getElementById('web_btn').innerText = data.btn_web ? "ON" : "OFF";

                pvHistory.push(data.pv);
                pvHistory.shift();

                spHistory.push(data.sp);
                spHistory.shift();

                drawGraph();
            });
        }

        function sendSetpoint(val) {
            fetch('/setpoint?val=' + val);
        }

        function toggleWebBtn() {
            fetch('/toggle');
        }

        setInterval(fetchData, 300);
    </script>
</body>
</html>
)rawliteral";

// HTTP Route Handlers
void handleRoot() {
  server.send_P(200, "text/html", index_html);
}

void handleData() {
  String json = "{\"pv\":" + String(pvValue) +
                ",\"sp\":" + String(setpointValue) +
                ",\"error\":" + String(errorValue) +
                ",\"dac\":" + String(dacValue) +
                ",\"btn_hw\":" + String(stateBtnHW) +
                ",\"btn_web\":" + String(stateBtnWeb) + "}";

  server.send(200, "application/json", json);
}

void handleSetpoint() {
  if (server.hasArg("val")) {
    setpointValue = server.arg("val").toInt();
    setpointValue = constrain(setpointValue, 0, 4095);
  }

  server.send(200, "text/plain", "OK");
}

void handleToggle() {
  stateBtnWeb = !stateBtnWeb;
  server.send(200, "text/plain", "OK");
}

void setup() {
  Serial.begin(115200);

  pinMode(PIN_BTN_HW, INPUT_PULLUP);

  // Initialize OLED
  Wire.begin(21, 22);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("Failed to initialize SSD1306 OLED"));
    for (;;);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Connecting to WiFi...");
  display.display();

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // HTTP Web Server Routing
  server.on("/", HTTP_GET, handleRoot);
  server.on("/data", HTTP_GET, handleData);
  server.on("/setpoint", HTTP_GET, handleSetpoint);
  server.on("/toggle", HTTP_GET, handleToggle);

  server.begin();
}

void loop() {
  server.handleClient();

  // 1. Read Process Variable (PV) from the potentiometer
  // ADC resolution: 12-bit (0 - 4095)
  pvValue = analogRead(PIN_POT);

  // 2. Calculate Error (Error = Set Point - Process Variable)
  errorValue = setpointValue - pvValue;

  // 3. Read Hardware Button (Active LOW)
  stateBtnHW = !digitalRead(PIN_BTN_HW);

  // 4. Scale DAC Output (8-bit: 0 - 255) from PV
  dacValue = map(pvValue, 0, 4095, 0, 255);

  // 5. Analog Voltage Output
  if (stateBtnHW || stateBtnWeb) {
    dacWrite(PIN_DAC, 255); // Maximum Override (approximately 3.3V)
  } else {
    dacWrite(PIN_DAC, dacValue); // Linear Output
  }

  // 6. Non-blocking OLED Display Update (200 ms interval)
  static unsigned long lastOLEDUpdate = 0;

  if (millis() - lastOLEDUpdate > 200) {
    display.clearDisplay();

    // Header: IP Address
    display.setCursor(0, 0);
    display.print("IP: ");
    display.println(WiFi.localIP());
    display.drawLine(0, 9, 128, 9, SSD1306_WHITE);

    // Set Point and Process Variable
    display.setCursor(0, 13);
    display.print("SP :");
    display.print(setpointValue);
    display.print(" | PV:");
    display.println(pvValue);

    // Error Calculation
    display.setCursor(0, 25);
    display.print("Err:");
    display.print(errorValue);
    display.print(" (");
    display.print((errorValue / 4095.0) * 3.3, 1);
    display.println("V)");

    // DAC Output
    display.setCursor(0, 37);
    display.print("DAC Out : ");
    display.print(dacValue);
    display.print(" (");
    display.print((dacValue / 255.0) * 3.3, 1);
    display.println("V)");

    // Button Status
    display.setCursor(0, 50);
    display.print("HW:");
    display.print(stateBtnHW ? "ON " : "OFF");
    display.print(" | WEB:");
    display.print(stateBtnWeb ? "ON" : "OFF");

    display.display();

    lastOLEDUpdate = millis();
  }
}
