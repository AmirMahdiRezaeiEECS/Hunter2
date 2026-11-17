# Tutorial 01 — Modular Programming in C++

## 1. What is modular programming?

Modular programming means dividing a program into small, independent parts called **modules**. Each module has one clear responsibility and communicates with other modules through a defined interface.

For example, a robot-car program can be divided into:

- **Motor module:** controls the motors.
- **Lights module:** controls indicators and lights.
- **Web module:** receives commands from the browser.
- **Main program:** initializes the modules and coordinates the system.

The goal is not simply to create more files. The goal is to keep responsibilities separate.

## 2. The two important C++ file types

### Header files (`.h`)

A header usually declares what a module provides: classes, functions, constants, and types.

```cpp
// motor_control.h
class MotorController {
public:
    void begin();
    void stop();
};
```

This tells other files that `MotorController` exists and provides `begin()` and `stop()`.

### Source files (`.cpp`)

A source file contains the implementation — how the declared functions work.

```cpp
// motor_control.cpp
#include "motor_control.h"

void MotorController::stop() {
    // Stop the motors here.
}
```

The `MotorController::` part means that `stop()` belongs to the `MotorController` class.

## 3. How files work together

A typical module has this relationship:

```text
main program
    |
    | includes
    v
motor_control.h  <---- declares the interface
    ^
    | implements
motor_control.cpp <--- contains the implementation
```

The main program includes the header. The compiler compiles the `.cpp` implementation, and the linker connects the pieces into the final program.

Use `#include "file.h"` for your own project headers. Use angle brackets for library headers, such as `#include <Arduino.h>`.

## 4. Why use classes?

A class groups related data and functions into one unit.

```cpp
MotorController motors;

motors.begin();
motors.stop();
```

Here, `motors` is an object created from the `MotorController` class. The object provides a clear interface without exposing the implementation details.

You do not have to use classes for every module. A group of related functions can also form a module. Classes are useful when a module owns state, such as the current light or motor state.

## 5. How this project is organized

```text
esp8266_modular_robot_car/
├── esp8266_modular_robot_car.ino
├── secrets.example.h
├── secrets.h                 # create this locally; do not commit it
├── src/
│   ├── config.h
│   ├── motor_control.h
│   ├── motor_control.cpp
│   ├── lights.h
│   ├── lights.cpp
│   ├── web_interface.h
│   └── web_interface.cpp
├── docs/
│   ├── architecture.md
│   └── hardware-notes.md
└── tutorials/
    └── 01-modular-programming-in-cpp.md
```

- `main` initializes the hardware, connects to Wi-Fi, starts the web server, and runs the main loop.
- `motor_control` owns motor-control behavior.
- `lights` owns indicator and rear-light behavior.
- `web_interface` handles HTTP routes, interprets browser commands, and builds the control page.
- `config.h` centralizes pin assignments and shared settings.
- `secrets.h` stores local Wi-Fi credentials.

## 6. The most important design rule: low coupling, high cohesion

**High cohesion:** everything inside a module belongs together. Motor-related behavior belongs in the motor module.

**Low coupling:** modules should depend on each other as little as practical. The web module can ask the motor module to execute a command without knowing which GPIO pins control the motors.

This makes the project easier to test, understand, modify, and extend.

## 7. A practical workflow for adding a feature

Suppose you want to add a headlight:

1. Decide which module owns the feature. In this project, that is `lights`.
2. Add the required pin/configuration.
3. Add a public method to `lights.h`, such as `setHeadlight(bool enabled)`.
4. Implement it in `lights.cpp`.
5. Call that method from the appropriate command handler in `web_interface.cpp`.
6. Update the documentation.

Avoid putting all the new code into the `.ino` file. The main file should coordinate modules, not contain every implementation.

## 8. Build and test

1. Open the project folder's `.ino` file in the Arduino IDE.
2. Install/select the ESP8266 board platform and select your NodeMCU board.
3. Copy `secrets.example.h` to `secrets.h` and enter your Wi-Fi credentials.
4. Verify and upload the sketch.
5. Open Serial Monitor at **115200 baud** and visit the printed IP address.

Arduino IDE compiles the sketch and its source files. Keep the `.ino` file in the project folder and keep the `.h`/`.cpp` files in `src/`.

**Remember:** modular programming is about separating responsibilities and defining clean interfaces — not about maximizing the number of files.
