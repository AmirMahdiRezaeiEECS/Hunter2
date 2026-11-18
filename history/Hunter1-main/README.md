# Wi-Fi-Controlled Robot Car Using ESP8266

**New:** an installable Android version of the proportional dual-joystick controller now lives in [`android/`](android/README.md) — see [`android/README.md`](android/README.md) for the prebuilt APK and setup steps.

## 1. Project Overview

In this project, we use an ESP8266 (NodeMCU) to control a two-motor
robot car through a Wi-Fi network.

-   \*The ESP8266 acts as a web server.A user connects to the same Wi-Fi
    network and controls the robot using a web page containing
    directional buttons. \*\*

The project has five main components:

1.  ESP8266: The main controller.
2.  L298N: The motor driver responsible for controlling the two DC
    motors.
3.  DC Motors: Responsible for moving the robot.
4.  LEDs: Used as indicators and vehicle lights.
5.  Web Interface (HTML): Provides a graphical control panel that allows
    the user to operate the robot through a web browser.

The general communication process is:

1.  Wi-Fi Connection: The ESP8266 connects to an existing Wi-Fi network
    in Station (STA) mode.
2.  Web Server Initialization: The ESP8266 starts an HTTP web server
    that listens for incoming client requests.
3.  Accessing the Web Server: The user enters the ESP8266's local IP
    address into a web browser.
4.  Displaying the Control Interface: The browser receives and displays
    the robot's HTML-based control interface.
5.  Sending Commands: When the user presses a control button, the
    browser sends an HTTP request containing the corresponding command.
6.  Processing Requests: The ESP8266 receives the request, extracts the
    command, and determines the required action.
7.  Controlling the Robot: Based on the received command, the ESP8266
    sends appropriate signals to the L298N motor driver and LEDs,
    controlling the robot's movement and lighting.

------------------------------------------------------------------------

## 2. Include the Required Libraries

``` cpp
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
```

We need two libraries to implement Wi-Fi communication and create an
HTTP server.

### Library 1: ESP8266WiFi.h

This library provides the functions required to connect the ESP8266 to a
Wi-Fi network.

For example:

-   `WiFi.begin()`
-   `WiFi.status()`
-   `WiFi.localIP()`
-   `WiFi.mode()`

### Library 2: ESP8266WebServer.h

This library allows the ESP8266 to operate as an HTTP web server.

It provides functions for:

-   Receiving HTTP requests.
-   Defining URL routes.
-   Sending HTTP responses.
-   Handling browser requests.

------------------------------------------------------------------------

## 3. Configure the Wi-Fi Network

First, define the Wi-Fi network credentials:

``` cpp
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";
```

-   `ssid`: The name of the Wi-Fi network.
-   `password`: The password required to connect to that network.

These credentials allow the ESP8266 to connect to an existing Wi-Fi
network.

### Create the Web Server

``` cpp
ESP8266WebServer server(80);
```

Here, we create an HTTP server object named `server`.

The number 80 represents the HTTP port.

### What is a port?

A port is a logical communication endpoint used by network applications.

For example:

-   HTTP: Port 80.
-   HTTPS: Port 443.

Therefore, the ESP8266 listens for HTTP requests on port 80.

------------------------------------------------------------------------

## 4. Define the Hardware Pins

Next, we assign ESP8266 GPIO pins to the motor driver and other
components.

### 4.1. Motor Driver Pins

The L298N motor driver controls two DC motors.

Each motor requires:

-   Two direction-control signals.
-   One PWM signal for speed control.

**Motor A:**

``` cpp
const byte L298N_A_pin = D1;
const byte L298N_A_In1_pin = D2;
const byte L298N_A_In2_pin = D3;
```

**Motor B:**

``` cpp
const byte L298N_B_In3_pin = D4;
const byte L298N_B_In4_pin = D5;
const byte L298N_B_pin = D6;
```

The pins have the following responsibilities:

  Pin   Function
  ----- ---------------------
  D1    Motor A speed (PWM)
  D2    Motor A direction
  D3    Motor A direction
  D4    Motor B direction
  D5    Motor B direction
  D6    Motor B speed (PWM)

The PWM signals control motor speed, while the digital signals determine
motor direction.

### 4.2. LED and Horn Pins

``` cpp
const byte Led1_pin = D7;
const byte Led2_pin = D8;
const byte Led3_pin = D9;
const byte Led4_pin = D10;
const byte Korna_pin = D0;
```

These pins are assigned to the robot's lighting system and horn.

  Pin   Function
  ----- -----------------
  D7    Left indicator
  D8    Right indicator
  D9    Front light
  D10   Rear light
  D0    Horn

**Important hardware warning:** On common ESP8266 NodeMCU boards, D9 and
D10 correspond to UART RX and TX pins. Using them for LEDs can interfere
with serial communication and uploading firmware. Check your board's
pinout before using them.

### 4.3. Define the Indicator Variables

``` cpp
byte SolSinyal = 0;
byte SagSinyal = 0;
byte ArkaLamba = 0;
```

These variables store the current state of the indicators.

-   `SolSinyal`: Left indicator status.
-   `SagSinyal`: Right indicator status.
-   `ArkaLamba`: Rear light status.

A value of 0 represents OFF, while 1 represents ON.

------------------------------------------------------------------------

## 5. The setup() Function

The `setup()` function executes only once when the ESP8266 starts or
resets.

We use it to configure the hardware, establish the Wi-Fi connection, and
initialize the web server.

### 5.1. Configure Motor Direction Pins

``` cpp
pinMode(L298N_A_In1_pin, OUTPUT);
pinMode(L298N_A_In2_pin, OUTPUT);
pinMode(L298N_B_In3_pin, OUTPUT);
pinMode(L298N_B_In4_pin, OUTPUT);
```

The `pinMode()` function configures a GPIO pin.

Its general syntax is:

``` cpp
pinMode(pin, mode);
```

Where:

-   `pin`: The GPIO pin we want to configure.
-   `mode`: The operating mode.

Common modes include:

  Mode           Meaning
  -------------- -----------------------------------------
  INPUT          Reads an external signal
  OUTPUT         Sends a digital signal
  INPUT_PULLUP   Input with an internal pull-up resistor

Here, we use OUTPUT because the ESP8266 sends signals to the L298N.

### 5.2. Set the Initial Motor State

``` cpp
digitalWrite(L298N_A_In1_pin, LOW);
digitalWrite(L298N_A_In2_pin, LOW);
digitalWrite(L298N_B_In3_pin, LOW);
digitalWrite(L298N_B_In4_pin, LOW);
```

The `digitalWrite()` function sets a digital output pin to HIGH or LOW.

-   HIGH: Logic HIGH.
-   LOW: Logic LOW.

Here, we initially set all motor direction pins to LOW.

This prevents the motors from receiving an unintended direction command
during initialization.

### 5.3. Configure LED Pins

``` cpp
pinMode(Led1_pin, OUTPUT);
pinMode(Led2_pin, OUTPUT);
pinMode(Led3_pin, OUTPUT);
pinMode(Led4_pin, OUTPUT);
```

We configure all four LED pins as outputs.

Then:

``` cpp
digitalWrite(Led1_pin, LOW);
digitalWrite(Led2_pin, LOW);
digitalWrite(Led3_pin, LOW);
digitalWrite(Led4_pin, LOW);
```

This ensures that all LEDs are initially turned off.

### 5.4. Initialize Serial Communication

``` cpp
Serial.begin(115200);
```

This statement initializes serial communication between the ESP8266 and
a computer.

### What is Serial?

Serial communication is a method of transmitting data one bit at a time
between two devices.

In this project, we use it mainly for debugging and monitoring the
ESP8266.

The number 115200 represents the baud rate, which specifies the
communication speed in bits per second.

For example:

``` cpp
Serial.println("Hello");
```

Prints a message in the Serial Monitor.

This is useful for checking whether the ESP8266 connects successfully to
Wi-Fi and for monitoring incoming commands.

------------------------------------------------------------------------

## 6. Connect the ESP8266 to Wi-Fi

Now we configure the ESP8266 to connect to an existing wireless network.

``` cpp
WiFi.mode(WIFI_STA);
```

The `WiFi.mode()` function determines the operating mode of the ESP8266.

Common modes include:

  Mode          Description
  ------------- ---------------------------------------
  WIFI_STA      Connects to an existing Wi-Fi network
  WIFI_AP       Creates its own Wi-Fi access point
  WIFI_AP_STA   Operates in both modes simultaneously

We use WIFI_STA because our robot connects to an existing Wi-Fi network.

### Start the Connection

``` cpp
WiFi.begin(ssid, password);
```

This function starts the connection process using the previously defined
credentials.

### Monitor the Connection

``` cpp
Serial.println("");
while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}
```

Let's understand this section.

1.  `WiFi.status()`

    Returns the current Wi-Fi connection status.

2.  `WL_CONNECTED`

    Represents a successful Wi-Fi connection.

3.  `while loop`

    The loop continues executing while the ESP8266 is not connected.

4.  `delay(500)`

    Pauses program execution for 500 milliseconds.

5.  `Serial.print(".")`

    Prints a dot to the Serial Monitor every 500 milliseconds.

This gives us a visual indication that the ESP8266 is still attempting
to connect.

Once the connection succeeds, the loop terminates.

**Note:** Your original code contains a syntax error here: WL_CONNECTED
/). The correct expression is WL_CONNECTED).

### Display Connection Information

``` cpp
Serial.println("");
Serial.print("Connected to ");
Serial.println(ssid);
Serial.print("IP address: ");
Serial.println(WiFi.localIP());
```

After connecting, we display:

-   The connected Wi-Fi network.
-   The ESP8266's local IP address.

The IP address is particularly important because we use it to access the
robot's web interface.

------------------------------------------------------------------------

## 7. Configure the HTTP Server

Now that the ESP8266 is connected to Wi-Fi, we need to define how it
responds to browser requests.

We use the `server.on()` function.

Its general syntax is:

``` cpp
server.on(URL, HTTP_handler);
```

A handler is a function that executes when the server receives a request
matching a particular URL.

### 7.1. Define the Root Route

``` cpp
server.on("/", handleRoot);
```

The `/` represents the root URL.

When a browser accesses the ESP8266's IP address, the `handleRoot()`
function executes.

### 7.2. Define the Car Control Route

``` cpp
server.on("/car", handleCar);
```

This route is responsible for receiving the robot's movement commands.

For example:

``` text
http://192.168.1.100/car?a=2
```

Here:

-   `/car` identifies the route.
-   `a=2` represents the movement command.

The `handleCar()` function processes this request.

### 7.3. Define an Additional Route

``` cpp
server.on("/inline",  {
server.send(200, "text/plain", "this works as well");
});
```

This example uses a lambda function.

A lambda is an anonymous function, meaning a function without a
conventional name.

Here, the server responds with a simple text message.

The `server.send()` function sends an HTTP response.

Its general syntax is:

``` cpp
server.send(statusCode, contentType, content);
```

For example:

``` cpp
server.send(200, "text/plain", "Hello!");
```

Where:

-   200: HTTP status code indicating success.
-   text/plain: Response content type.
-   "Hello!": Response body.

### 7.4. Handle Unknown URLs

``` cpp
server.onNotFound(handleNotFound);
```

This function specifies what happens when a browser requests a URL that
hasn't been defined.

In our project, `handleNotFound()` generates a 404 response.

### 7.5. Start the Server

``` cpp
server.begin();
Serial.println("HTTP server started");
```

`server.begin()` starts the HTTP server.

After this statement, the ESP8266 can receive HTTP requests.

------------------------------------------------------------------------

## 8. The loop() Function

``` cpp
void loop() {
server.handleClient();
}
```

Unlike `setup()`, the `loop()` function executes repeatedly.

The `server.handleClient()` function checks for incoming HTTP client
requests and processes them.

For example:

1.  A user presses the forward button.
2.  The browser sends an HTTP request.
3.  The ESP8266 receives the request.
4.  `server.handleClient()` processes it.
5.  The corresponding handler executes.

Without `server.handleClient()`, the web server would not properly
process incoming requests in this programming model.

------------------------------------------------------------------------

## 9. Motor Control Function

One of the most important functions in this project is:

``` cpp
void motorSpeed(
int prmA,
byte prmA1,
byte prmA2,
int prmB,
byte prmB1,
byte prmB2
) {
analogWrite(L298N_A_pin, prmA);
analogWrite(L298N_B_pin, prmB);
digitalWrite(L298N_A_In1_pin, prmA1);
digitalWrite(L298N_A_In2_pin, prmA2);
digitalWrite(L298N_B_In3_pin, prmB1);
digitalWrite(L298N_B_In4_pin, prmB2);
}
```

This function controls both motors.

It receives six parameters.

  Parameter   Function
  ----------- ---------------------------
  prmA        Motor A speed
  prmA1       Motor A direction input 1
  prmA2       Motor A direction input 2
  prmB        Motor B speed
  prmB1       Motor B direction input 1
  prmB2       Motor B direction input 2

### Understanding analogWrite()

``` cpp
analogWrite(L298N_A_pin, prmA);
```

The `analogWrite()` function generates a PWM signal.

PWM stands for Pulse Width Modulation.

PWM controls the average power delivered to a load by changing the duty
cycle of a digital signal.

For example:

-   Low duty cycle → Lower average motor power.
-   High duty cycle → Higher average motor power.

On the ESP8266 Arduino core, the default PWM range is 0--1023.

Therefore, the values 90 and 100 in your code represent relatively low
duty cycles.

### Understanding digitalWrite()

``` cpp
digitalWrite(L298N_A_In1_pin, prmA1);
digitalWrite(L298N_A_In2_pin, prmA2);
```

These statements determine the direction of Motor A.

The L298N uses two input signals to determine motor direction.

  IN1    IN2    Motor behavior
  ------ ------ ----------------------------------
  LOW    LOW    Stop / coast
  HIGH   LOW    Rotate in one direction
  LOW    HIGH   Rotate in the opposite direction
  HIGH   HIGH   Brake

The same principle applies to Motor B.

------------------------------------------------------------------------

## 10. HTTP Request Handlers

We now examine the functions responsible for responding to browser
requests.

### 10.1. Root Handler

``` cpp
void handleRoot() {
server.send(200, "text/plain", "hello from esp8266!");
}
```

When a user accesses the root URL, the ESP8266 sends a simple text
response.

### 10.2. Car Control Handler

This is the main function responsible for controlling the robot.

``` cpp
void handleCar() {
String message = "";
int BtnValue = 0;
```

First, we declare two variables.

-   `message`: Stores the HTML content that will be sent to the browser.
-   `BtnValue`: Stores the movement command received from the browser.

### 10.3. Read HTTP Parameters

``` cpp
for (uint8_t i = 0; i < server.args(); i++) {
if (server.argName(i) == "a") {
String s = server.arg(i);
BtnValue = s.toInt();
}
Serial.println(
server.argName(i) + ": " + server.arg(i) + "\n"
);
}
```

Let's understand the important functions.

  Function            Description
  ------------------- --------------------------------------
  server.args()       Returns the number of HTTP arguments
  server.argName(i)   Returns the name of an argument
  server.arg(i)       Returns the value of an argument
  toInt()             Converts a String into an integer

Suppose the browser sends:

``` text
/car?a=2
```

The ESP8266 receives:

``` text
Argument name: a
Argument value: 2
```

Then:

``` cpp
BtnValue = s.toInt();
```

Converts the string "2" into the integer 2.

Now the ESP8266 knows which movement command was requested.

------------------------------------------------------------------------

## 11. Control Robot Movement Using switch-case

We use a switch-case statement to execute different actions depending on
the received command.

``` cpp
switch (BtnValue) {
```

The program checks the value of `BtnValue` and executes the
corresponding case.

### Movement Commands

  Value   Command         Description
  ------- --------------- ------------------------------------
  1       Left turn       Turns left
  2       Forward         Moves forward
  3       Right turn      Turns right
  4       Full left       Rotates left
  5       Stop            Stops both motors
  6       Full right      Rotates right
  7       Left reverse    Moves backward while turning left
  8       Reverse         Moves backward
  9       Right reverse   Moves backward while turning right

### Example 1: Move Forward

``` cpp
case 2:
motorSpeed(100, HIGH, LOW, 100, HIGH, LOW);
SolSinyal = 0;
SagSinyal = 0;
digitalWrite(Led1_pin, LOW);
digitalWrite(Led2_pin, LOW);
break;
```

Both motors receive:

-   Speed: 100.
-   Direction input 1: HIGH.
-   Direction input 2: LOW.

Therefore, both motors rotate in the same direction.

The indicators are also turned off.

### Example 2: Turn Left

``` cpp
case 1:
motorSpeed(90, LOW, LOW, 100, HIGH, LOW);
SolSinyal = 1;
digitalWrite(Led1_pin, HIGH);
break;
```

Here:

-   Motor A is stopped.
-   Motor B rotates forward.
-   The left indicator is activated.

This creates a turning movement.

### Example 3: Stop

``` cpp
case 5:
motorSpeed(0, LOW, LOW, 0, LOW, LOW);
SolSinyal = 0;
SagSinyal = 0;
digitalWrite(Led1_pin, LOW);
digitalWrite(Led2_pin, LOW);
break;
```

Both motors receive zero PWM duty cycle.

Consequently, the motors stop receiving driving power.

------------------------------------------------------------------------

## 12. Control the Rear Lights and Indicators

After the switch-case statement, we have:

``` cpp
if (BtnValue > 7) {
ArkaLamba = 1;
SolSinyal = 1;
SagSinyal = 1;
digitalWrite(Led1_pin, HIGH);
digitalWrite(Led2_pin, HIGH);
digitalWrite(Led4_pin, HIGH);
} else {
ArkaLamba = 0;
digitalWrite(Led4_pin, LOW);
}
```

This section checks whether the received command is greater than 7.

If it is, the program activates:

-   Left indicator.
-   Right indicator.
-   Rear light.

Otherwise, the rear light is turned off.

**Important:** The variables `SolSinyal`, `SagSinyal`, and `ArkaLamba`
store indicator states, but they do not automatically make the LEDs
blink.

For that, we need another function.

------------------------------------------------------------------------

## 13. Generate HTML for the Robot Control Interface

One interesting feature of this project is that the ESP8266 generates
the web page itself.

We don't need a separate web server or external website.

Inside `handleCar()`, we construct an HTML document using a String
variable.

For example:

``` cpp
message += "<html>";
message += "<head><title>Robotics</title></head>";
message += "<body>";
message += "<h3>WiFi Robot Car NodeMCU Web Server</h3>";
```

Here, we begin constructing an HTML document.

### Create a Forward Button

``` cpp
message += "<a href=\"/car?a=2\">";
message += "<button>^</button>";
message += "</a>";
```

This creates a button that sends the following request when clicked:

``` text
/car?a=2
```

Remember that command 2 represents forward movement.

### Create a Stop Button

``` cpp
message += "<a href=\"/car?a=5\">";
message += "<button>Stop</button>";
message += "</a>";
```

When the user clicks this button, the browser sends:

``` text
/car?a=5
```

The ESP8266 receives the command and stops the motors.

### Create the Other Direction Buttons

Your original code creates nine buttons:

  Button   HTTP Request
  -------- --------------
  ↖        `/car?a=1`
  ↑        `/car?a=2`
  ↗        `/car?a=3`
  ←        `/car?a=4`
  Stop     `/car?a=5`
  →        `/car?a=6`
  ↙        `/car?a=7`
  ↓        `/car?a=8`
  ↘        `/car?a=9`

Finally, we close the HTML document and send it to the browser:

``` cpp
message += "</body></html>";
server.send(200, "text/html", message);
```

The browser receives the HTML content and displays the robot control
interface.

------------------------------------------------------------------------

## 14. Indicator Blinking Function

Your code also contains:

``` cpp
void tempSinyal() {
if (SolSinyal == 1)
digitalWrite(Led1_pin, !digitalRead(Led1_pin));
if (SagSinyal == 1)
digitalWrite(Led2_pin, !digitalRead(Led2_pin));
if (ArkaLamba == 1)
digitalWrite(Led4_pin, !digitalRead(Led4_pin));
}
```

This function toggles the LED states.

Let's examine:

``` cpp
digitalWrite(Led1_pin, !digitalRead(Led1_pin));
```

It consists of three operations:

1.  `digitalRead()` reads the current LED output state.
2.  `!` reverses the logical value.
3.  `digitalWrite()` writes the opposite state.

For example:

  Previous state   New state
  ---------------- -----------
  HIGH             LOW
  LOW              HIGH

Therefore, repeatedly calling this function makes the LEDs blink.

**Important:** There is a problem in your original implementation.

You never call `tempSinyal()` inside `loop()`.

Consequently, your indicators won't blink using this function.

You would need to call it periodically, preferably using `millis()`
rather than blocking delays.

------------------------------------------------------------------------

## 15. Handle Unknown Requests

Finally, we have:

``` cpp
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
message += " " + server.argName(i) + ": ";
message += server.arg(i) + "\n";
}
server.send(404, "text/plain", message);
}
```

This function handles requests to undefined URLs.

For example, if someone accesses:

``` text
http://192.168.1.100/unknown
```

The ESP8266 returns a 404 error.

The response contains:

-   Requested URL.
-   HTTP method.
-   Number of arguments.
-   Names and values of the arguments.

This information is useful for debugging HTTP requests.

------------------------------------------------------------------------

## 16. Important Problems in the Original Code

Before considering the project finished, there are several things you
should correct.

  -----------------------------------------------------------------------
  Problem                             Explanation
  ----------------------------------- -----------------------------------
  Incorrect Wi-Fi condition           WL_CONNECTED /) is a syntax error.

  PWM pin configuration               Configure the two L298N enable pins
                                      as OUTPUT.

  D9 and D10                          These overlap with UART pins on
                                      common NodeMCU boards.

  Indicator blinking                  `tempSinyal()` is never called.

  Root page                           `/` displays only a text message;
                                      the control interface is at `/car`.

  Horn                                `Korna_pin` is declared but never
                                      configured or used.

  Front LED                           `Led3_pin` is configured but never
                                      used.

  PWM values                          ESP8266's default PWM range is
                                      0--1023, not 0--255.
  -----------------------------------------------------------------------

One additional point: Your current implementation uses HTTP GET requests
to control motors. That's fine for a basic demonstration, but a
production robot should have additional safety measures, including a
command timeout so that the motors stop if communication is interrupted.

------------------------------------------------------------------------

## 17. Overall Project Architecture

Let's summarize the entire project:

``` text
              Wi-Fi Network
                   |
                   |
              ESP8266
                   |
            HTTP Web Server
                   |
            Incoming Request
                   |
              handleCar()
                   |
            Read HTTP Argument
                   |
              BtnValue
                   |
              switch-case
                   |
          +--------+--------+
          |                 |
     Motor Control      LED Control
          |                 |
       L298N              LEDs
          |
     DC Motors
```

### The Most Important Functions to Understand

  Function             Responsibility
  -------------------- ------------------------------------
  `setup()`            Initializes hardware and Wi-Fi
  `loop()`             Processes incoming HTTP requests
  `motorSpeed()`       Controls motor speed and direction
  `handleRoot()`       Handles the root URL
  `handleCar()`        Processes movement commands
  `tempSinyal()`       Toggles indicator LEDs
  `handleNotFound()`   Handles undefined URLs

My recommendation for learning this project: Don't try to memorize the
entire code. Understand these three concepts first:

1.  Wi-Fi and HTTP: How the ESP8266 communicates with a browser.
2.  Motor control: How PWM and digital signals control the L298N.
3.  Request handling: How an HTTP parameter such as a=2 becomes a
    physical action.

Once you understand these three concepts, the rest of the code becomes
much easier to follow.
