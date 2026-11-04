// display_oled.cpp
#include "display_oled.h"

static Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

bool initDisplay() {
    Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
    if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
        return false;
    }
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(F("Initializing..."));
    display.display();
    return true;
}

void updateDisplay(const SystemState& state, const String& ipAddress) {
    display.clearDisplay();
    
    // Header Section: IP Address
    display.setCursor(0, 0);
    display.print(F("IP: "));
    display.println(ipAddress);
    display.drawLine(0, 9, 128, 9, SSD1306_WHITE);

    // ADC & DAC Readings
    display.setCursor(0, 14);
    display.print(F("ADC (12b): ")); 
    display.println(state.adcRawValue);
    
    display.setCursor(0, 26);
    display.print(F("DAC (8b) : ")); 
    display.println(state.dacOutputValue);

    display.setCursor(0, 38);
    display.print(F("Voltage  : ")); 
    display.print(state.analogVoltage, 2); 
    display.println(F(" V"));

    // Button Status Indicators
    display.setCursor(0, 52);
    display.print(F("HW:")); 
    display.print(state.hardwareBtnState ? F("ON ") : F("OFF"));
    display.print(F(" | WEB:")); 
    display.print(state.webBtnState ? F("ON") : F("OFF"));

    display.display();
}