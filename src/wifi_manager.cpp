#include <ESP8266WiFi.h>

#include "wifi_manager.h"
#include "../secrets.h"

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