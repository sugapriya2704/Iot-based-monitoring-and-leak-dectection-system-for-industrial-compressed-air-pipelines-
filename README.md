# IoT-Based Monitoring and Leak Detection System for Industrial Compressed Air Pipelines

## Overview

This project is an ESP32-based IoT system designed to monitor industrial compressed air pipelines and identify abnormal pressure and flow conditions associated with air leakage.

The system uses two BMP280 pressure sensors and two YF-S401 flow sensors to monitor pressure and flow at different stages of the pipeline.

## Features

* Real-time pressure monitoring
* Real-time flow monitoring
* Stage-wise leak detection
* Buzzer alert during leakage
* OLED local display
* Blynk IoT remote monitoring
* ESP32-based control and data processing
* Serial Monitor for real-time debugging

## Hardware Used

* ESP32
* BMP280 Pressure Sensors × 2
* YF-S401 Flow Sensors × 2
* 0.96-inch OLED Display
* Buzzer
* Industrial compressed air pipeline setup
* Connecting wires and power supply

## Software and Platforms

* Arduino IDE
* Blynk IoT
* Embedded C/C++
* I2C Communication

## Working Principle

The two BMP280 sensors measure pressure at different points of the compressed air pipeline, while the YF-S401 sensors measure the corresponding flow rates.

The ESP32 continuously collects the sensor data and compares the pressure and flow conditions. Based on predefined threshold conditions, the system identifies:

1. Normal condition
2. Leak in Stage 1
3. Leak in Stage 2
4. System OFF condition

When a leak condition is detected, the buzzer provides a local alert. The pressure, flow rate, and leak status are also displayed on the OLED and transmitted to the Blynk IoT platform for remote monitoring.

## Blynk Virtual Pins

| Virtual Pin | Parameter        |
| ----------- | ---------------- |
| V0          | Stage 1 Pressure |
| V1          | Stage 2 Pressure |
| V2          | Stage 1 Flow     |
| V3          | Stage 2 Flow     |
| V4          | Leak Status      |

## Applications

* Industrial compressed air systems
* Pipeline monitoring
* Preventive maintenance
* Industrial IoT
* Energy-efficiency monitoring
* Leak detection systems

## Future Scope

* Automatic leak localization
* Leakage quantity estimation
* Efficiency calculation
* Data logging and historical analysis
* Machine learning-based anomaly detection
* Industrial dashboard integration
* Automatic maintenance alerts

## Security

Wi-Fi credentials and Blynk authentication tokens are stored separately in `secrets.h`. The file is excluded from Git using `.gitignore`.

## Author

Sugapriya M
B.E. Electronics and Communication Engineering
