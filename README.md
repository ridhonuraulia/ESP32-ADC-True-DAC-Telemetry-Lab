# ESP32-ADC-True-DAC-Telemetry-Lab
The ESP32 ADC &amp; True DAC Telemetry Lab is an open-source project designed for embedded systems enthusiasts, IoT developers, and engineering students. This project demonstrates 12-bit analog sampling (ADC1), 8-bit true analog voltage generation (True DAC1), SSD1306 OLED visualization, and real-time web telemetry streaming..
# ESP32 ADC & True DAC Telemetry Lab

A modular, production-ready embedded C++ project for the **ESP32-WROOM-32** demonstrating hardware Analog-to-Digital Converter (ADC1) signal sampling, 8-bit R-2R True Digital-to-Analog Converter (DAC1) voltage generation, SSD1306 OLED visual feedback, and a real-time web telemetry control server.

---

## Key Features

* **Hardware True DAC Output:** Uses GPIO 25 (DAC1) to output true, continuous DC voltage levels ($0.0\text{V} - 3.3\text{V}$), bypassing traditional PWM pulse-width modulation.
* **Isolated ADC Sampling:** Configured exclusively on **ADC1 (GPIO 34)** to prevent hardware channel conflicts with the ESP32 Wi-Fi stack (ADC2 constraint).
* **Non-Blocking Architecture:** Driven by a state-machine timing loop (`millis()`) to prevent Wi-Fi dropouts and ensure sub-millisecond web API responsiveness.
* **SSD1306 OLED Dashboard:** 128x64 pixel display with frame-buffered telemetry UI updated at 5 Hz.
* **Embedded Web Telemetry Server:** Asynchronous AJAX-driven UI rendered from ESP32 Flash memory (`PROGMEM`) featuring real-time HTML5 Canvas signal plotting.
* **Clean Modular C++ Design:** Modular separation across Hardware Abstraction Layers (`adc_driver`, `dac_driver`), UI rendering (`display_oled`), Network APIs (`web_server_app`), and central state synchronization (`config.h`).

---

## Hardware Requirements & Pinout

### Component List

1. **ESP32-WROOM-32** Development Board (30-pin or 38-pin variant).
2. **Potentiometer** (10k$\Omega$ recommended).
3. **SSD1306 OLED Display** (0.96 inch, $128 \times 64$, I2C address `0x3C`).
4. **Push Button** (Tactile switch).
5. **LED** + $220\Omega$ Current-Limiting Resistor (or Multimeter / Oscilloscope to verify DAC output).
6. Breadboard and Jumper Wires.

### Connection Pinout Table

| ESP32 Pin | Component | Module Pin | Description / Notes |
| --- | --- | --- | --- |
| **GPIO 34** | Potentiometer | Center Wiper | ADC1 Channel (Input Voltage $0 - 3.3\text{V}$) |
| **GPIO 25** | LED Anode / Meter | Positive Terminal | **DAC1 Channel** (True Analog Voltage Output) |
| **GPIO 14** | Push Button | Terminal 1 | Hardware Toggle (Internal Pull-Up, Active LOW) |
| **GPIO 21** | OLED Display | SDA | I2C Data Line |
| **GPIO 22** | OLED Display | SCL | I2C Clock Line |
| **3.3V** | Potentiometer / OLED | VCC | $3.3\text{V}$ Power Rail |
| **GND** | All Components | GND | Common Ground |

---

## Software Architecture

The application isolates hardware abstraction, networking, and display rendering by passing a single `SystemState` reference across modular drivers.

```
                  +-------------------------+
                  |  Potentiometer (3.3V)   |
                  +------------+------------+
                               |
                               v
                       [ ADC1: GPIO 34 ]
                               |
                               v
                   +-----------------------+
                   |  readADC() (12-bit)   |
                   +-----------+-----------+
                               |
                               v
+-------------------------------------------------------------------+
|                        SystemState Struct                         |
|  - adcRawValue    : uint16_t (0 - 4095)                           |
|  - dacOutputValue : uint8_t  (0 - 255)                            |
|  - analogVoltage  : float    (0.0 - 3.3V)                         |
|  - hardwareBtnState: bool                                         |
|  - webBtnState    : bool                                          |
+------+-----------------------+--------------------------------+
       |                       |                                |
       v                       v                                v
+--------------+       +---------------+               +----------------+
|  True DAC    |       |  OLED Display |               |   Web Server   |
|  (GPIO 25)   |       |   (SSD1306)   |               |   REST / JSON  |
+--------------+       +---------------+               +----------------+
  Analog Output          128x64 Frame                    HTML5 Canvas 
  0.0V - 3.3V            Buffer (5 Hz)                   Plotting (3 Hz)

```

---

## Repository Directory Structure

```text
esp32-adc-dac-lab/
├── esp32-adc-dac-lab.ino    # Main application entry point & execution loop
├── config.h                 # Central configuration, pin assignments, & SystemState struct
├── adc_driver.h             # ADC Hardware Abstraction Layer interface
├── adc_driver.cpp           # ADC Initialization & sampling driver
├── dac_driver.h             # True DAC Abstraction Layer interface
├── dac_driver.cpp           # DAC Output control driver
├── display_oled.h           # SSD1306 Display driver interface
├── display_oled.cpp         # Frame buffer UI rendering engine
├── web_pages.h              # Static HTML5, CSS, and JS web application (PROGMEM)
├── web_server_app.h         # Web Server API routing interface
└── web_server_app.cpp       # REST Endpoint handlers and JSON serialization
└── image                    # For documentation 

```

---

## REST API Specification

The embedded web server provides endpoints for telemetry streaming and control:

| Endpoint | Method | Response Type | Description |
| --- | --- | --- | --- |
| `/` | `GET` | `text/html` | Serves the single-page HTML5 web application from flash. |
| `/api/data` | `GET` | `application/json` | Returns current system telemetry state. |
| `/api/toggle` | `POST` / `GET` | `text/plain` | Toggles the web control button state (`webBtnState`). |

### JSON Telemetry Payload Example (`GET /api/data`)

```json
{
  "adc": 2048,
  "dac": 127,
  "btn_hw": 0,
  "btn_web": 1
}

```

---

## Building and Flashing

### Option A: Arduino IDE

1. Open **Arduino IDE** (v2.0+ recommended).
2. Install **ESP32 Board Support**:
* Open `Preferences` $\rightarrow$ Add Additional Board Manager URL:
`[https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json](https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json)`
* Go to `Board Manager` $\rightarrow$ Search **esp32** $\rightarrow$ Install.


3. Install Library Dependencies via `Library Manager`:
* **Adafruit GFX Library**
* **Adafruit SSD1306**


4. Open `esp32-adc-dac-lab.ino`.
5. Edit `config.h` to update `WIFI_SSID` and `WIFI_PASSWORD`.
6. Select Board: `ESP32 Dev Module`.
7. Click **Upload**.

---

### Option B: PlatformIO (VS Code)

If compiling via PlatformIO, create a `platformio.ini` file in the root directory:

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
monitor_speed = 115200
lib_deps =
    adafruit/Adafruit GFX Library @ ^1.11.9
    adafruit/Adafruit SSD1306 @ ^2.5.9

```

Run compilation and upload:

```bash
pio run --target upload
pio device monitor

```

---

## License

Distributed under the MIT License. See `LICENSE` for more information.
