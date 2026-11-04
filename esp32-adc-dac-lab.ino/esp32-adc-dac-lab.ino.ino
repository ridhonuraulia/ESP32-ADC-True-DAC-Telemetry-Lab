#include "config.h"
#include "adc_driver.h"
#include "dac_driver.h"
#include "display_oled.h"
#include "web_server_app.h"
#include <WiFi.h>

SystemState systemState = {0, 0, 0.0f, false, false};
String localIPStr = "0.0.0.0";

void setup() {
    Serial.begin(115200);
    
    pinMode(PIN_BTN_HW, INPUT_PULLUP);
    
    initADC();
    initDAC();

    if (!initDisplay()) {
        Serial.println(F("CRITICAL ERROR: OLED display failed to initialize!"));
        while (true) delay(100);
    }

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print(F("Connecting to WiFi network"));
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(F("."));
    }
    
    localIPStr = WiFi.localIP().toString();
    Serial.println(F("\nWiFi Connected successfully."));
    Serial.print(F("IP Address: "));
    Serial.println(localIPStr);

    initWebServer(systemState);
}

void loop() {
    // 1. Process incoming HTTP web client requests
    handleWebRequests();

    // 2. Read hardware inputs
    systemState.adcRawValue = readADC();
    systemState.hardwareBtnState = !digitalRead(PIN_BTN_HW); // Active LOW due to INPUT_PULLUP

    // 3. Scale 12-bit ADC (0-4095) to 8-bit DAC (0-255)
    systemState.dacOutputValue = map(systemState.adcRawValue, 0, 4095, 0, 255);
    systemState.analogVoltage = (systemState.dacOutputValue / 255.0f) * 3.3f;

    // 4. Drive True DAC hardware output
    if (systemState.hardwareBtnState || systemState.webBtnState) {
        writeDAC(255); // Force full 3.3V saturation on active button state
    } else {
        writeDAC(systemState.dacOutputValue); // Linear mapping
    }

    // 5. Non-blocking OLED display update Task (Every 200 ms)
    static unsigned long lastOLEDUpdateTick = 0;
    if (millis() - lastOLEDUpdateTick >= OLED_REFRESH_INTERVAL_MS) {
        updateDisplay(systemState, localIPStr);
        lastOLEDUpdateTick = millis();
    }
}