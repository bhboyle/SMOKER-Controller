# Smoker Web Server - ESP32-S3

A web-based control and monitoring system for a pellet smoker, running on ESP32-S3. This firmware provides a mobile-friendly web interface for monitoring and controlling your smoker remotely.

![Smoker Controller](https://img.shields.io/badge/Platform-ESP32--S3-blue)
![License](https://img.shields.io/badge/License-MIT-green)

## Overview

This project is the web server component of a two-board smoker controller system:
- **ESP32-S3** (this project): Handles WiFi, web interface, and temperature sensing
- **ESP32-P4** (display controller): Handles the physical display, fan control, and PID regulation

The ESP32-S3 reads temperatures from MAX6675 thermocouple sensors and sends them to the display controller, while also providing a responsive web dashboard for remote monitoring and control.

## Features

- **Real-time Monitoring**
  - Smoker temperature display
  - Food probe temperature display
  - Fan RPM and PWM output visualization
  - Cook timer with start/stop/reset controls
  - 2-hour temperature history chart

- **Remote Control**
  - Set target temperature
  - Toggle fan on/off
  - Adjust PID parameters (Kp, Ki, Kd)
  - Timer controls

- **Temperature Calibration**
  - Adjustable offset for smoker probe (-50 to +50°F)
  - Adjustable offset for food probe (-50 to +50°F)
  - Calibration values stored in NVram (persistent across power cycles)

- **Connectivity**
  - WiFi with AP fallback mode for initial setup
  - mDNS support (access via `smoker.local`)
  - WebSocket for real-time updates (1-second refresh)
  - REST API for integration

- **System**
  - OTA (Over-The-Air) firmware updates via ElegantOTA
  - Persistent settings stored in NVram
  - Mobile-friendly responsive web design

## Hardware Requirements

### Main Components
- **ESP32-S3-DevKitC-1** (N16R8 configuration)
  - 16MB Flash
  - 8MB PSRAM
- **2x MAX6675** thermocouple amplifier modules
- **K-type thermocouples** for smoker and food probes
- **ESP32-P4** display controller (separate board)

### Pin Configuration

| Function           | GPIO Pin |
|--------------------|----------|
| SPI CLK (MAX6675)  | GPIO 12  |
| SPI MISO (MAX6675) | GPIO 13  |
| Smoker Probe CS    | GPIO 10  | 
| Food Probe CS      | GPIO 11  |
| Controller TX      | GPIO 17  |
| Controller RX      | GPIO 18  |

## Installation


### Building with Arduino IDE
1. Install ESP32 board support via Board Manager
2. Select "ESP32S3 Dev Module" as the board
3. Configure Flash Size: 16MB
4. Open `SmokerWebServer.ino` and upload

## Initial Setup

### WiFi Configuration

On first boot (or when no WiFi credentials are stored), the ESP32-S3 creates an access point:

| Setting    |   Value       |
|------------|---------------|
| SSID       | `SmokerSetup` |
| Password   | `smoker123`   |
| IP Address | `192.168.4.1` |

1. Connect to the `SmokerSetup` WiFi network
2. Open a browser and navigate to `http://192.168.4.1`
3. Expand the "WiFi Settings" section
4. Enter your WiFi credentials and hostname
5. Click "Save & Connect"

The device will restart and connect to your network.

### Accessing the Dashboard

After WiFi configuration:
- **mDNS**: `http://smoker.local` (or your custom hostname)
- **IP Address**: Check your router or serial monitor for the assigned IP

## Web Interface

### Temperature Display
Shows current readings for:
- **Smoker**: Internal smoker temperature
- **Food**: Food probe temperature
- **Set Temp**: Target temperature

### Controls
- **Set Temperature**: Use arrow buttons to adjust, then click "Set Temp"
- **Fan Control**: Toggle switch to enable/disable the fan
- **RPM/PWM Bars**: Visual indicators of fan speed and duty cycle

### Timer
- **Start/Stop**: Toggle the cook timer
- **Reset**: Reset timer to 00:00:00

### Temperature History
A 2-hour rolling chart showing smoker and food temperatures, updated every 2 minutes.

### Advanced Settings

#### Temperature Calibration
Adjust probe offsets to correct for sensor inaccuracies:
- **Positive values**: Add degrees (if sensor reads low)
- **Negative values**: Subtract degrees (if sensor reads high)
- Range: -50°F to +50°F
- Values are saved to NVram and persist across reboots

#### PID Settings
Fine-tune the temperature control algorithm:
- **Kp**: Proportional gain (default: 7.0)
- **Ki**: Integral gain (default: 0.0)
- **Kd**: Derivative gain (default: 0.2)

### WiFi Settings
Change WiFi network or hostname without reflashing.

## API Reference

### REST Endpoints

| Endpoint           | Method |          Description         |
|--------------------|--------|------------------------------|
| `/`                | GET    | Main web dashboard           |
| `/api/state`       | GET    | Current smoker state (JSON)  |
| `/api/command`     | POST   | Send command to controller   |
| `/api/calibration` | GET    | Get calibration offsets      |
| `/api/calibration` | POST   | Set calibration offsets      |
| `/wifi/status`     | GET    | WiFi connection status       | 
| `/wifi`            | POST   | Configure WiFi settings      |
| `/update`          | GET    | OTA update page (ElegantOTA) |

### WebSocket

Connect to `/ws` for real-time state updates.

**State JSON Format:**
```json
{
  "controllerConnected": true,
  "smokerTemp": 225,
  "foodTemp": 145,
  "setTemp": 225,
  "rpm": 1500,
  "pwm": 128,
  "fanOn": true,
  "timerMs": 3600000,
  "Kp": 7.0,
  "Ki": 0.0,
  "Kd": 0.2,
  "smokerOffset": 0,
  "foodOffset": -5,
  "smokerHistory": [220, 222, 225, ...],
  "foodHistory": [140, 142, 145, ...],
  "historyCount": 30
}
```

### Command Format

Send commands via WebSocket or POST to `/api/command`:
```json
{"command": "SET_TEMP:250"}
{"command": "FAN:ON"}
{"command": "FAN:OFF"}
{"command": "TIMER:START"}
{"command": "TIMER:RESET"}
{"command": "PID:7.0:0.0:0.2"}
{"command": "GET_CONFIG"}
```

## Serial Protocol

Communication with the ESP32-P4 display controller:

| Direction       | Format                                    |                  Example                        |
|-----------------|-------------------------------------------|-------------------------------------------------|
| To Controller   | `CMD:command`                             | `CMD:SET_TEMP:250`                              |
| To Controller   | `TEMPS:smoker,food`                       | `TEMPS:225,145`                                 |
| From Controller | `DATA:s,f,set,rpm,pwm,fan,timer,Kp,Ki,Kd` | `DATA:-1,-1,225,1500,128,1,3600000,7.0,0.0,0.2` |

**Note:** The display controller sends `-1` for temperature values since the ESP32-S3 reads them locally.

## Partition Layout

Custom partition table optimized for OTA updates and data storage:

| Partition | Size  |              Purpose                       |
|-----------|-------|--------------------------------------------|
| NVS       | 24KB  | WiFi credentials, calibration, preferences |
| PHY_INIT  | 4KB   | WiFi calibration data                      |
| OTA Data  | 8KB   | Tracks active app partition                |
| App0      | 3MB   | Primary application                        |
| App1      | 3MB   | OTA update partition                       |
| LittleFS  | 9.8MB | Future database storage                    |
| Coredump  | 64KB  | Crash debugging                            |

## OTA Updates

1. Navigate to `http://smoker.local/update`
2. Select the new firmware `.bin` file
3. Click "Update"
4. Wait for the device to restart

## Troubleshooting

### Cannot connect to WiFi
- Ensure credentials are correct
- Check if the network is 2.4GHz (ESP32 doesn't support 5GHz)
- Try resetting by erasing flash and reconfiguring

### Temperature reads -999
- Check thermocouple connections
- Verify MAX6675 wiring (CS, CLK, MISO pins)
- Ensure thermocouples are not open circuit

### Web interface not updating
- Check WebSocket connection status
- Verify the ESP32 is connected to WiFi
- Try refreshing the page

### Controller shows "Not Connected"
- Check serial connection between ESP32-S3 and P4
- Verify TX/RX pins are correctly wired (TX to RX, RX to TX)
- Ensure both boards are powered

## Dependencies

- [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer)
- [AsyncTCP](https://github.com/me-no-dev/AsyncTCP)
- [ArduinoJson](https://arduinojson.org/)
- [ElegantOTA](https://github.com/ayushsharma82/ElegantOTA)
- ESP32 Preferences library (built-in)

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Author

Created with assistance from Claude (Anthropic)

## Acknowledgments

- ESP32 Arduino Core developers
- AsyncWebServer library maintainers
- ElegantOTA for simple OTA updates
