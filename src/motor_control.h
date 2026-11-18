#pragma once

#include <Arduino.h>

class MotorController {
public:
  void begin();
  void setSpeed(int leftPercent, int rightPercent);
  void executeCommand(int command);
  void stop();

private:
  void setMotorSpeeds(uint8_t speedA, uint8_t inA1, uint8_t inA2,
                      uint8_t speedB, uint8_t inB1, uint8_t inB2);
};
