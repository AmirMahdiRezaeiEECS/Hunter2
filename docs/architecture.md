# Project Architecture

## Overview

The ESP8266 hosts an HTTP server. A browser sends a command to `/car?a=<number>`. The web module interprets that command and calls the motor and light modules.

```text
Browser
   |
   | HTTP GET /car?a=2
   v
ESP8266WebServer
   |
   v
WebInterface
   |             |
   |             +------> VehicleLights
   v
MotorController

Main sketch initializes all modules and services the web server.
```

## Module responsibilities

| File | Responsibility |
|---|---|
| `esp8266_modular_robot_car.ino` | Program entry point, Wi-Fi connection, initialization, main loop |
| `src/config.h` | Pin assignments and shared constants |
| `src/motor_control.h/.cpp` | Motor initialization, direction, speed, stop |
| `src/lights.h/.cpp` | Indicator and rear-light state, non-blocking indicator flashing |
| `src/web_interface.h/.cpp` | HTTP routes, command parsing, HTML control page |
| `secrets.h` | Local Wi-Fi credentials; excluded from version control |

## HTTP commands

| Query | Action |
|---|---|
| `/car?a=1` | Left |
| `/car?a=2` | Forward |
| `/car?a=3` | Right |
| `/car?a=4` | Sharp left |
| `/car?a=5` | Stop |
| `/car?a=6` | Sharp right |
| `/car?a=7` | Reverse-left |
| `/car?a=8` | Reverse |
| `/car?a=9` | Reverse-right |

## Main execution flow

1. `setup()` initializes Serial, motors, and lights.
2. The ESP8266 connects to the configured Wi-Fi network.
3. `web.begin()` registers HTTP routes, then the server starts.
4. `loop()` services incoming HTTP clients and updates the indicator blink state.
5. The web handler passes movement commands to `MotorController` and light states to `VehicleLights`.

The indicator update uses `millis()` rather than `delay()`, so flashing does not deliberately block the main loop.
