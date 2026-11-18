#pragma once

#include <Arduino.h>

// Full PWM duty for a 100% drive command (ESP8266 analogWrite range)
constexpr uint8_t MAX_MOTOR_SPEED = 255;

// Motor driver (L298N)
constexpr uint8_t MOTOR_A_PWM = D1;
constexpr uint8_t MOTOR_A_IN1 = D2;
constexpr uint8_t MOTOR_A_IN2 = D3;

constexpr uint8_t MOTOR_B_IN1 = D4;
constexpr uint8_t MOTOR_B_IN2 = D5;
constexpr uint8_t MOTOR_B_PWM = D6;

// Indicator and rear light outputs
constexpr uint8_t LEFT_INDICATOR_PIN  = D7;
constexpr uint8_t RIGHT_INDICATOR_PIN = D8;
constexpr uint8_t REAR_LIGHT_PIN      = D0;

constexpr uint16_t INDICATOR_INTERVAL_MS = 400;
