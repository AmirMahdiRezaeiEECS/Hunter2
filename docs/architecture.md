# Project Architecture

## Overview

The ESP8266 hosts an HTTP server. Two clients are supported:

- The Robot Controller app sends `GET /control?left=<-100..100>&right=<-100..100>`
  (~16 Hz). Negative percentages reverse a motor; zero stops it. This is the
  primary interface.
- The built-in browser page sends `/car?a=<number>` discrete commands
  (the fallback interface).

In both cases the web module interprets the request and calls the motor and
light modules.

```text
Browser / Controller app
   |
   | HTTP GET /control?left=80&right=-40   (or /car?a=2)
   v
ESP8266WebServer
   |
   v
WebInterface
   |             |
   |             +------> VehicleLights
   v
MotorController
   left: -100..100, right: -100..100

Main sketch initializes all modules and services the web server.
```

## Module responsibilities

| File | Responsibility |
|---|---|
| `Hunter2.ino` | Program entry point, Wi-Fi connection, initialization, main loop |
| `src/config.h` | Pin assignments and shared constants |
| `src/motor_control.h/.cpp` | Motor initialization, direction, speed, stop; `setSpeed(-100..100, -100..100)` differential drive |
| `src/lights.h/.cpp` | Indicator and rear-light state, non-blocking indicator flashing |
| `src/web_interface.h/.cpp` | HTTP routes, command parsing, HTML control page |
| `secrets.h` | Local Wi-Fi credentials; excluded from version control |

## HTTP commands

| Query | Action |
|---|---|
| `/control?left=100&right=100` | Analog differential drive; each motor -100..100 percent (negative = reverse) |
| `/car?a=1` | Left |
| `/car?a=2` | Forward |
| `/car?a=3` | Right |
| `/car?a=4` | Sharp left |
| `/car?a=5` | Stop |
| `/car?a=6` | Sharp right |
| `/car?a=7` | Reverse-left |
| `/car?a=8` | Reverse |
| `/car?a=9` | Reverse-right |

For `/control`, pivot turns (one side negative, the other positive) flash the
matching indicator, both sides negative turns on the rear light, and a full
stop clears both.

## Main execution flow

1. `setup()` initializes Serial, motors, and lights.
2. The ESP8266 connects to the configured Wi-Fi network.
3. `web.begin()` registers HTTP routes, then the server starts.
4. `loop()` services incoming HTTP clients and updates the indicator blink state.
5. The web handler passes movement commands to `MotorController` and light states to `VehicleLights`.

The indicator update uses `millis()` rather than `delay()`, so flashing does not deliberately block the main loop.
