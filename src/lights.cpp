#include "lights.h"
#include "config.h"

void VehicleLights::begin() {
  pinMode(LEFT_INDICATOR_PIN, OUTPUT);
  pinMode(RIGHT_INDICATOR_PIN, OUTPUT);
  pinMode(REAR_LIGHT_PIN, OUTPUT);

  digitalWrite(LEFT_INDICATOR_PIN, LOW);
  digitalWrite(RIGHT_INDICATOR_PIN, LOW);
  digitalWrite(REAR_LIGHT_PIN, LOW);
}

void VehicleLights::setIndicators(bool left, bool right) {
  leftEnabled = left;
  rightEnabled = right;

  if (!leftEnabled && !rightEnabled) {
    indicatorsOn = false;
    digitalWrite(LEFT_INDICATOR_PIN, LOW);
    digitalWrite(RIGHT_INDICATOR_PIN, LOW);
  }
}

void VehicleLights::setRearLight(bool enabled) {
  rearEnabled = enabled;
  digitalWrite(REAR_LIGHT_PIN, rearEnabled ? HIGH : LOW);
}

void VehicleLights::update() {
  const unsigned long now = millis();

  if (now - lastToggle < INDICATOR_INTERVAL_MS) {
    return;
  }

  lastToggle = now;
  indicatorsOn = !indicatorsOn;

  digitalWrite(LEFT_INDICATOR_PIN,
               leftEnabled && indicatorsOn ? HIGH : LOW);
  digitalWrite(RIGHT_INDICATOR_PIN,
               rightEnabled && indicatorsOn ? HIGH : LOW);
}
