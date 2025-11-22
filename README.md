# CAN Bus Control & Temperature Monitoring System

A dual-transceiver system using ESP32 and CAN bus for bidirectional communication between a master controller and a slave position control system.

## System Overview

```
┌─────────────────┐                    ┌──────────────────┐
│  Master (ESP32) │                    │  Slave (ESP32)   │
│                 │                    │                  │
│  Potentiometer  │  ──── CAN Bus ──>  │  DAC Output      │
│  (Setpoint)     │                    │  (Position Ctrl) │
│                 │                    │                  │
│  WiFi Server    │  <─── CAN Bus ───  │  MAX6675         │
│  (Temperature)  │                    │  (Thermocouple)  │
└─────────────────┘                    └──────────────────┘
```

## Features

### Master Node (`transceiver_master.ino`)
- **Setpoint Transmission**: Reads potentiometer voltage (0-3.3V) and sends via CAN bus
- **Temperature Reception**: Receives temperature data from slave via CAN
- **Web Integration**: Forwards temperature data to HTTP server every 5 seconds
- **Error Handling**: Acknowledgment-based retry mechanism (up to 3 retries)

### Slave Node (`transceiver_slave.ino`)
- **Setpoint Reception**: Receives voltage setpoint via CAN and outputs to DAC (GPIO25)
- **Temperature Monitoring**: Reads MAX6675 thermocouple sensor
- **Temperature Transmission**: Sends temperature data to master via CAN
- **Auto-ACK**: Automatically acknowledges received setpoint messages

## Hardware Requirements

### Master Node
- ESP32 development board
- MCP2515 CAN controller module (CS: GPIO5)
- Potentiometer (connected to GPIO13)
- WiFi connectivity

### Slave Node
- ESP32 development board
- MCP2515 CAN controller module (CS: GPIO5)
- MAX6675 thermocouple module (SCK: GPIO14, CS: GPIO15, MISO: GPIO12)
- DAC output on GPIO25

## CAN Bus Configuration

- **Bitrate**: 500 kbps
- **MCP2515 Clock**: 8 MHz
- **CAN IDs**:
  - `0x036` - Voltage/Setpoint data
  - `0x037` - Acknowledgment
  - `0x038` - Temperature data

## Setup Instructions

1. **Configure WiFi** (Master only):
   ```cpp
   #define WIFI_SSID "YOUR_WIFI_SSID"
   #define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
   #define WEB_SERVER_URL "http://192.168.1.100:3000/api/temperature"
   ```

2. **Wire CAN Bus**:
   - Connect CAN_H and CAN_L between both MCP2515 modules
   - Add 120Ω termination resistors at both ends

3. **Upload Code**:
   - Upload `transceiver_master.ino` to master ESP32
   - Upload `transceiver_slave.ino` to slave ESP32

4. **Monitor Serial Output**: Open Serial Monitor at 115200 baud on both devices

## Data Format

- **Voltage**: Transmitted as 16-bit integer (millivolts)
- **Temperature**: Transmitted as 16-bit integer (hundredths of °C)
  - Example: `2550` = 25.50°C

## Communication Protocol

1. Master reads potentiometer → Sends setpoint via CAN
2. Slave receives setpoint → Outputs to DAC → Sends ACK
3. Slave reads temperature → Sends via CAN
4. Master receives temperature → Forwards to web server

## Dependencies

```
- SPI.h (built-in)
- mcp2515.h (CAN controller library)
- WiFi.h (ESP32 built-in)
- HTTPClient.h (ESP32 built-in)
- max6675.h (Thermocouple library)
```

## License

Educational project for Industrial Electronics course.
