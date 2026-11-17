#include "web_interface.h"

WebInterface::WebInterface(ESP8266WebServer& serverRef,
                           MotorController& motorRef,
                           VehicleLights& lightsRef)
    : server(serverRef), motors(motorRef), lights(lightsRef) {}

void WebInterface::begin() {
  server.on("/", [this]() { handleRoot(); });
  server.on("/car", [this]() { handleCar(); });
  server.onNotFound([this]() { handleNotFound(); });
}

void WebInterface::handleRoot() {
  server.send(200, "text/html", makeControlPage());
}

void WebInterface::handleCar() {
  const int command = server.arg("a").toInt();

  motors.executeCommand(command);

  // Commands 1 and 4 indicate left; 3 and 6 indicate right.
  // Reverse commands turn on the rear light.
  const bool left = (command == 1 || command == 4);
  const bool right = (command == 3 || command == 6);
  const bool reverse = (command >= 7 && command <= 9);

  if (command == 2 || command == 5) {
    lights.setIndicators(false, false);
  } else {
    lights.setIndicators(left || reverse, right || reverse);
  }

  lights.setRearLight(reverse);

  server.send(200, "text/html", makeControlPage());
}

String WebInterface::makeControlPage() {
  String page;
  page.reserve(1800);

  page += F("<!doctype html><html><head><meta name='viewport' "
            "content='width=device-width,initial-scale=1'>");
  page += F("<title>Wi-Fi Robot Car</title>");
  page += F("<style>body{font-family:Arial;text-align:center}"
            "a{display:inline-block;margin:6px}"
            "button{width:82px;height:70px;font-size:24px}</style>");
  page += F("</head><body><h2>Wi-Fi Robot Car</h2>");

  const char* labels[] = {"↖", "↑", "↗", "←", "STOP", "→", "↙", "↓", "↘"};
  for (int i = 0; i < 9; ++i) {
    page += F("<a href='/car?a=");
    page += String(i + 1);
    page += F("'><button>");
    page += labels[i];
    page += F("</button></a>");
    if (i % 3 == 2) page += F("<br>");
  }

  page += F("</body></html>");
  return page;
}

void WebInterface::handleNotFound() {
  String message = F("404 - File Not Found\nURI: ");
  message += server.uri();
  message += F("\nMethod: ");
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += F("\nArguments: ");
  message += String(server.args());

  for (uint8_t i = 0; i < server.args(); ++i) {
    message += "\n";
    message += server.argName(i);
    message += ": ";
    message += server.arg(i);
  }

  server.send(404, "text/plain", message);
}
