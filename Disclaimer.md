## Project Overview & Disclaimer
The ESP32 ADC & True DAC Telemetry Lab is an open-source project designed for embedded systems enthusiasts, IoT developers, and engineering students. This project demonstrates 12-bit analog sampling (ADC1), 8-bit true analog voltage generation (True DAC1), SSD1306 OLED visualization, and real-time web telemetry streaming using a modular C++ architecture.

## Hardware Verification & References
This project was built by synthesizing online technical documentation, Espressif datasheets, and open-source embedded references. It is not raw copied theory—every component has been physically wired, debugged, and verified on ESP32-WROOM-32 hardware. Key hardware behaviors, such as ADC1 Wi-Fi channel isolation, DAC voltage output scaling, and non-blocking millis() execution, have been tested directly on a physical breadboard setup.

## Open-Source Intent & Disclaimer
Educational Purpose: Created to demonstrate the fundamental differences between PWM pseudo-analog and True DAC hardware output, along with modular C++ software design principles on ESP32.

Open Use: Shared under the MIT License. Anyone is free to use, modify, distribute, or integrate this code for academic, personal, or commercial projects.

Disclaimer (Use at Your Own Risk): This software is provided "as-is" without warranty of any kind. The author assumes no liability for hardware damage, incorrect wiring, or fried microcontroller pins during replication. Always verify pin voltage limits before connecting external circuitry.

Contributions, issue reports, and pull requests are welcome!
