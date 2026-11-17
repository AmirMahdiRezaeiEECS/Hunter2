#pragma once

#include <ESP8266WebServer.h>
#include "motor_control.h"
#include "lights.h"

class WebInterface {
public:
  WebInterface(ESP8266WebServer& server,
               MotorController& motors,
               VehicleLights& lights);

  void begin();

private:
  ESP8266WebServer& server;
  MotorController& motors;
  VehicleLights& lights;

  void handleRoot();
  void handleCar();
  void handleNotFound();
  String makeControlPage();
};
