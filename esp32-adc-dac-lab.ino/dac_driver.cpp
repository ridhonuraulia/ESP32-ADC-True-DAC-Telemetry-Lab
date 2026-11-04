// dac_driver.cpp
#include "dac_driver.h"

void initDAC() {
    // GPIO 25 is automatically configured when dacWrite is invoked
}

void writeDAC(uint8_t value) {
    dacWrite(PIN_DAC_OUT, value);
}