#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include "src/config.h"
#include "src/motor_control.h"
#include "src/lights.h"
#include "src/web_interface.h"
#include "secrets.h"

ESP8266WebServer server(80);

MotorController motors;
VehicleLights lights;
WebInterface web(server, motors, lights);

void connectToWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Connected to: ");
  Serial.println(WIFI_SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

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
