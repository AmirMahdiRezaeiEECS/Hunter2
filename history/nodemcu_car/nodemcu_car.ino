#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "EECS";
const char* password = "62996299";

ESP8266WebServer server(80);

const byte L298N_A_pin = D1;      // GPI05
const byte L298N_A_In1_pin = D2;  //GPI04
const byte L298N_A_In2_pin = D3;  //GPI0

const byte L298N_B_In3_pin = D4;  // GPI02
const byte L298N_B_In4_pin = D5;  // GPIO14
const byte L298N_B_pin = D6;      //GPI12

const byte Led1_pin = D7;   //GPIO13  // سیگنال چرخش چپ
const byte Led2_pin = D8;   //GPIO15  //راهنمای سمت راست
const byte Led3_pin = D9;   //GPIO3   // چراغ جلو
const byte Led4_pin = D10;  //GPI01   // چراغ عقب
const byte Korna_pin = D0;  //GPI016  //جلو
byte leftIndicator = 0;
byte rightIndicator = 0;
byte rearLight = 0;

void setup() {
  
  pinMode(L298N_A_In1_pin, OUTPUT);
  pinMode(L298N_A_In2_pin, OUTPUT);
  pinMode(L298N_B_In3_pin, OUTPUT);
  pinMode(L298N_B_In4_pin, OUTPUT);

  digitalWrite(L298N_A_In1_pin, LOW);
  digitalWrite(L298N_A_In2_pin, LOW);
  digitalWrite(L298N_B_In3_pin, LOW);
  digitalWrite(L298N_B_In4_pin, LOW);

  pinMode(Led1_pin, OUTPUT);
  pinMode(Led2_pin, OUTPUT);
  pinMode(Led3_pin, OUTPUT);
  pinMode(Led4_pin, OUTPUT);

  digitalWrite(Led1_pin, LOW);
  digitalWrite(Led2_pin, LOW);
  digitalWrite(Led3_pin, LOW);
  digitalWrite(Led4_pin, LOW);

  Serial.begin(115200);

  WiFi.mode(WIFI_STA);  //WIFI_STA: Configures the ESP8266 as a Station that means Connects to an existing Wi-Fi network.
  WiFi.begin(ssid, password);
  Serial.println("");  //This is mainly for readability.

  /*Check the connection status*/
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/car", handleCar);

  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}

void motorSpeed(int prmA, byte prmA1, byte prmA2, int prmB, byte prmB1, byte prmB2) {
  analogWrite(L298N_A_pin, prmA);
  analogWrite(L298N_B_pin, prmB);

  digitalWrite(L298N_A_In1_pin, prmA1);
  digitalWrite(L298N_A_In2_pin, prmA2);
  digitalWrite(L298N_B_In3_pin, prmB1);
  digitalWrite(L298N_B_In4_pin, prmB2);
}

void handleRoot() {
  server.send(200, "text/plain", "hello from esp8266!");
}

void handleCar() {
  String message = "";
  int BtnValue = 0;
  for (uint8_t i = 0; i < server.args(); i++) {
    if (server.argName(i) == "a") {
      String s = server.arg(i);
      BtnValue = s.toInt();
    }
    Serial.println(server.argName(i) + ": " + server.arg(i) + "\n");
  }

  switch (BtnValue) {
    case 1:  // sola donuş
      motorSpeed(90, LOW, LOW, 100, HIGH, LOW);
      leftIndicator = 1;
      digitalWrite(Led1_pin, HIGH);
      break;
    case 2:  // düz ileri
      motorSpeed(100, HIGH, LOW, 100, HIGH, LOW);
      leftIndicator = 0;
      rightIndicator = 0;
      digitalWrite(Led1_pin, LOW);
      digitalWrite(Led2_pin, LOW);
      break;
    case 3:  // saga donuş
      motorSpeed(100, HIGH, LOW, 90, LOW, LOW);
      rightIndicator = 1;
      digitalWrite(Led2_pin, HIGH);
      break;
    case 4:  // tam sola donuş
      motorSpeed(90, LOW, HIGH, 90, HIGH, LOW);
      leftIndicator = 1;
      digitalWrite(Led1_pin, HIGH);
      break;
    case 5:  // stop
      motorSpeed(0, LOW, LOW, 0, LOW, LOW);
      leftIndicator = 0;
      rightIndicator = 0;
      digitalWrite(Led1_pin, LOW);
      digitalWrite(Led2_pin, LOW);
      break;
    case 6:  //
      motorSpeed(90, HIGH, LOW, 90, LOW, HIGH);
      rightIndicator = 1;
      digitalWrite(Led2_pin, HIGH);
      break;
    case 7:  //sol geri
      motorSpeed(90, LOW, LOW, 100, LOW, HIGH);
      break;
    case 8:  // tam geri
      motorSpeed(90, LOW, HIGH, 90, LOW, HIGH);
      break;
    case 9:  // sag geri
      motorSpeed(100, LOW, HIGH, 90, LOW, LOW);
    default:
      break;
  }
  if (BtnValue > 7) {
    rearLight = 1;
    leftIndicator = 1;
    rightIndicator = 1;
    digitalWrite(Led1_pin, HIGH);
    digitalWrite(Led2_pin, HIGH);
    digitalWrite(Led4_pin, HIGH);
  } else {
    rearLight = 0;
    digitalWrite(Led4_pin, LOW);
  }

  message += "<html> <head> <title> Robotics </title><head>";
  message += "<body><h3>Wifi Robot Car NodeMCU  Web Server</h1>";
  message += "<table> ";
  message += "<tr>";
  message += "<td><p><a href=\"/car?a=1\"><button style=\"width:100;height:100;font-size:100px;\" class=\"button\">\\</button></a></p> ";
  message += "<td><p><a href=\"/car?a=2\"><button style=\"width:100;height:100;font-size:100px;\" class=\"button\">^</button></a></p> ";
  message += "<td><p><a href=\"/car?a=3\"><button style=\"width:100;height:100;font-size:100px;\" class=\"button\">/</button></a></p> ";
  message += "<tr>";
  message += "<td><p><a href=\"/car?a=4\"><button style=\"width:100;height:100;font-size:100px;\" class=\"button\"> < </button></a></p> ";
  message += "<td><p><a href=\"/car?a=5\"><button style=\"width:100;height:100;font-size:40px;\" class=\"button\">Stop</button></a></p> ";
  message += "<td><p><a href=\"/car?a=6\"><button style=\"width:100;height:100;font-size:100px;\" class=\"button\"> > </button></a></p> ";
  message += "<tr>";
  message += "<td><p><a href=\"/car?a=7\"><button style=\"width:100;height:100;font-size:100px;\" class=\"button\">/</button></a></p> ";
  message += "<td><p><a href=\"/car?a=8\"><button style=\"width:100;height:100;font-size:100px;\" class=\"button\">v</button></a></p> ";
  message += "<td><p><a href=\"/car?a=9\"><button style=\"width:100;height:100;font-size:100px;\" class=\"button\">\\</button></a></p> ";
  message += "</table> ";
  message += "</body></html>";
  server.send(200, "text/html", message);
}

void tempSinyal() {
  if (leftIndicator == 1)
    digitalWrite(Led1_pin, !digitalRead(Led1_pin));

  if (rightIndicator == 1)
    digitalWrite(Led2_pin, !digitalRead(Led2_pin));

  if (rearLight == 1)
    digitalWrite(Led4_pin, !digitalRead(Led4_pin));
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}
