#pragma once

#include <Arduino.h>

class VehicleLights {
public:
  void begin();
  void setIndicators(bool left, bool right);
  void setRearLight(bool enabled);
  void update();

private:
  bool leftEnabled = false;
  bool rightEnabled = false;
  bool rearEnabled = false;
  bool indicatorsOn = false;
  unsigned long lastToggle = 0;
};
