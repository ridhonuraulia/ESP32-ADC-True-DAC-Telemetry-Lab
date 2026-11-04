// adc_driver.cpp
#include "adc_driver.h"

void initADC() {
    pinMode(PIN_ADC_POT, INPUT);
}

uint16_t readADC() {
    return analogRead(PIN_ADC_POT);
}