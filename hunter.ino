#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include "src/config.h"
#include "src/motor_control.h"
#include "src/lights.h"
#include "src/web_interface.h"
#include "src/wifi_manager.h"

ESP8266WebServer server(80);

MotorController motors;
VehicleLights lights;
WebInterface web(server, motors, lights);

void setup() {
    Serial.begin(115200);

    motors.begin();
    lights.begin();

    connectToWiFi();

    web.begin();
    server.begin();

    Serial.println("HTTP server started");
}

void loop() {
    server.handleClient();
    lights.update();
}