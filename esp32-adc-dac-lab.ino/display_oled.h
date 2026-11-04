// display_oled.h
#ifndef DISPLAY_OLED_H
#define DISPLAY_OLED_H

#include "config.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

bool initDisplay();
void updateDisplay(const SystemState& state, const String& ipAddress);

#endif