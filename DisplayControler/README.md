# Smoker Controller - Display Unit

An ESP32-P4 based touchscreen controller for monitoring and controlling a smoker. This unit provides the user interface with a 1024x600 LCD display and communicates with a separate ESP32-S3 web server module that handles temperature sensors and WiFi connectivity.

## Features

- **PID Temperature Control**: Maintains smoker temperature using a fan-based airflow control system
- **Dual Temperature Monitoring**: Displays both smoker chamber and food probe temperatures
- **Touchscreen Interface**: 1024x600 LCD with GT911 capacitive touch
- **Real-time Temperature Chart**: Graphical display of temperature history over time
- **Cook Timer**: Start/stop/reset timer for tracking cook duration
- **Fan Control**: Manual on/off control with RPM and drive percentage displays
- **Configurable Settings**: PID tuning parameters, display intervals, and UI settings stored in NVS (non-volatile storage)
- **Safety Watchdog**: Automatically disables fan if temperature data is not received within 10 seconds

![Smoker Controller](https://img.shields.io/badge/Platform-ESP32--P4-blue)
![License](https://img.shields.io/badge/License-MIT-green)

## Hardware Requirements

### Display Controller (This Unit)
- **MCU**: ESP32-P4
- **Display**: 1024x600 LCD with JD9165 driver
- **Touch**: GT911 capacitive touch controller
- **Fan Output**: PWM on GPIO 28
- **Tachometer Input**: GPIO 29 (fan RPM feedback)
- **Serial Communication**: UART1 on GPIO 33 (RX) / GPIO 34 (TX)

### Web Server (Separate ESP32-S3 Module)
- Handles MAX6675 thermocouple interfaces
- Provides WiFi web interface
- Sends temperature data via serial: `TEMPS:smokerTemp,foodTemp`

## Pin Configuration

| Function       | GPIO |
|----------------|------|
| LCD Reset      | 27   |
| LCD Backlight  | 23   |
| Touch SDA      | 7    |
| Touch SCL      | 8    |
| Fan PWM        | 28   |
| Fan Tachometer | 29   |
| Web Serial RX  | 33   |
| Web Serial TX  | 34   |

## Serial Protocol

### Messages FROM Web Server (ESP32-S3)
```
TEMPS:smokerTemp,foodTemp          # Temperature data (sent periodically)
CMD:SET_TEMP:225                   # Set target temperature
CMD:FAN:ON / CMD:FAN:OFF           # Control fan
CMD:TIMER:START/STOP/RESET         # Control timer
CMD:PID:Kp:Ki:Kd                   # Update PID tuning
CMD:DISPLAY_INT:500                # Set display update interval (ms)
CMD:CHART_INT:60000                # Set chart update interval (ms)
CMD:HOLD_TIME:5000                 # Button hold time before swap (ms)
CMD:SWAP_INT:2000                  # Container swap interval (ms)
CMD:FADE_DUR:100                   # Animation fade duration (ms)
CMD:GET_CONFIG                     # Request current configuration
```

### Messages TO Web Server (ESP32-S3)
```
DATA:smokerTemp,foodTemp,setTemp,rpm,pwm,fanOn,timerMs,Kp,Ki,Kd
CONFIG:dispInterval,chartInterval,buttonHold,swapInterval,fadeDuration,Kp,Ki,Kd
```

## Configuration Parameters

### PID Defaults
| Parameter | Default Value |
|-----------|---------------|
| Kp        | 7.0           |
| Ki        | 0.0           |
| Kd        | 0.2           |

### Temperature Limits
| Setting      | Value |
|--------------|-------|
| Minimum Temp | 150°F |
| Maximum Temp | 500°F |
| Startup Temp | 225°F |

### Timing Defaults
| Setting          | Default          |
|------------------|------------------|
| Display Update   | 500ms            |
| Chart Update     | 60000ms (1 min)  |
| Serial Update    | 500ms            |
| Watchdog Timeout | 10000ms (10 sec) |

## User Interface

The display is organized into several sections:

1. **Timer Section** (top-left): Displays elapsed cook time with START/STOP and RESET buttons
2. **Temperature Display** (left-center): Alternates between Actual Temp and Food Temp with cross-fade animation
3. **Set Temperature** (left-lower): Shows target temperature with UP/DOWN adjustment buttons
4. **Fan Control** (top-right): ON/OFF switch with RPM and Drive percentage bars
5. **Temperature Chart** (right): Real-time graph showing smoker (red) and food (orange) temperature trends

## Dependencies

- [LVGL](https://lvgl.io/) - Light and Versatile Graphics Library (v9.x)
- [PID_v1](https://github.com/br3ttb/Arduino-PID-Library) - Arduino PID Library
- ESP32 Arduino Core with Preferences library

## Building

This project is designed for the Arduino IDE or VSCode with the Arduino Maker Workshop with ESP32-P4 board support.

1. Install ESP32 board support (ESP32-P4 variant)
2. Install required libraries (LVGL, PID_v1)
3. Configure `lv_conf.h` for your display settings
4. Upload to ESP32-P4

## File Structure

```
DisplayControler/
├── DisplayControler.ino   # Main application
├── pins_config.h                          # GPIO pin definitions
├── style.h                                # LVGL style definitions
├── lv_conf.h                              # LVGL configuration
├── src/
│   ├── lcd/
│   │   ├── jd9165_lcd.cpp/.h              # LCD driver wrapper
│   │   └── esp_lcd_jd9165.c/.h            # Low-level LCD driver
│   └── touch/
│       ├── gt911_touch.cpp/.h             # Touch driver wrapper
│       ├── esp_lcd_touch_gt911.c/.h       # GT911 touch driver
│       └── esp_lcd_touch.c/.h             # Touch abstraction layer
└── *.c                                    # Custom fonts (Antonio, Arial)
```

## Safety Features

- **Temperature Watchdog**: If no temperature data is received from the ESP32-S3 for 10 seconds, the fan is automatically disabled to prevent uncontrolled heating
- **PID Output Limits**: Maximum fan PWM is capped at 255 for safety
- **Connection Status**: Display shows "NO LINK" when communication with the temperature sensor module is lost

## License

This project is for personal/educational use.
