#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// WiFi Configuration
#define WIFI_SSID     "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

// Hardware Pin Definitions
#define PIN_ADC_POT   34  // ADC1 Channel (GPIO 34)
#define PIN_DAC_OUT   25  // True DAC Channel 1 (GPIO 25)
#define PIN_BTN_HW    14  // Hardware Push Button

// Display Configuration (SSD1306 OLED)
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define OLED_ADDRESS  0x3C
#define PIN_I2C_SDA   21
#define PIN_I2C_SCL   22

// System Timing Intervals (ms)
#define OLED_REFRESH_INTERVAL_MS 200

// Shared Application State Structure
struct SystemState {
    uint16_t adcRawValue;    // 12-bit ADC (0 - 4095)
    uint8_t  dacOutputValue; // 8-bit DAC  (0 - 255)
    float    analogVoltage;  // Calculated real voltage (0.0 - 3.3V)
    bool     hardwareBtnState;
    bool     webBtnState;
};

#endif // CONFIG_H