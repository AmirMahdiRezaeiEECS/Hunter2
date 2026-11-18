# Hardware and Safety Notes

## Important pin-mapping issue

The original sketch used `D9` and `D10` for LEDs. On common ESP8266 NodeMCU boards, GPIO9 and GPIO10 are connected to the module's flash memory and are not general-purpose pins. This version does not use them.

This version assigns:
- D1–D6 to the L298N motor driver.
- D7 and D8 to left/right indicators.
- D0 to the rear light.

The original front-light and horn outputs are not included in this first modular version. The ESP8266 has a limited number of safely usable GPIO pins; adding outputs may require an I/O expander or a revised pin allocation.

## ESP8266 boot-strapping pins

Be careful with D3 (GPIO0), D4 (GPIO2), and D8 (GPIO15). Their electrical levels during reset affect whether the ESP8266 boots normally. External motor-driver inputs or attached circuits must not force these pins to the wrong levels during startup.

Test the board with the motor driver disconnected first. Check the ESP8266 board's pinout and the L298N module wiring before connecting power.

## Power and electrical safety

- Do not power DC motors directly from ESP8266 GPIO pins.
- Use the motor driver's appropriate motor supply.
- Connect the ESP8266 and motor-driver grounds together.
- Ensure the motor supply voltage and current are suitable for the motors and driver.
- Avoid powering motors from the ESP8266's 3.3 V output.
- Test with the wheels raised and keep a way to disconnect motor power.

## PWM

The motor speed values in this project use `analogWrite()`. On the ESP8266 Arduino core, PWM behavior and supported range depend on the core version/configuration. Confirm the expected PWM range for your installed core; the values here preserve the original sketch's 0–255-style assumptions.
