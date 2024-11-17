#include <WiFi.h>                   // Includes library for WiFi connection
#include <WebSocketsClient.h>       // Includes library for WebSocket client
#include <NewPing.h>                // Includes library for ultrasonic sensor

// Pin definitions for the ultrasonic sensors
#define TRIGGER_PIN_2 18            // Trigger pin for the second ultrasonic sensor
#define ECHO_PIN_2 5                // Echo pin for the second ultrasonic sensor
#define TRIGGER_PIN_1 21            // Trigger pin for the first ultrasonic sensor
#define ECHO_PIN_1 19               // Echo pin for the first ultrasonic sensor
#define RX_PIN 16                   // RX pin for UART communication
#define TX_PIN 17                   // TX pin for UART communication
#define MAX_DISTANCE 300            // Maximum distance measured by the ultrasonic sensor

const char* ssid = "Proximus-Home-3660";          // WiFi network name
const char* password = "wh9uy942afsk9";           // WiFi network password
const char* serverAddress = "192.168.1.33";       // IP address of your Node.js server
const uint16_t serverPort = 5000;                 // Port used by your Node.js server
bool deco = false;

WebSocketsClient webSocket;          // Creates WebSocket object
HardwareSerial uart(1);              // Uses hardware UART on RX_PIN and TX_PIN
NewPing sonar1(TRIGGER_PIN_1, ECHO_PIN_1, MAX_DISTANCE); // Initializes the first ultrasonic sensor
NewPing sonar2(TRIGGER_PIN_2, ECHO_PIN_2, MAX_DISTANCE); // Initializes the second ultrasonic sensor
unsigned long lastSendTime = 0;      // Timestamp of the last data sent
const unsigned long sendInterval = 500; // Time interval between data sends (500 ms)

String uartBuffer = "";              // Buffer to store data received via UART

void setup() {
  Serial.begin(115200);              // Initializes serial communication at 115200 baud
  uart.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN); // Initializes UART communication at 9600 baud

  connectWiFi();                     // Connects to WiFi network
  connectWebSocket();                // Connects to WebSocket server
}

void loop() {
  webSocket.loop();                  // Handles WebSocket communication
  readUART();                        // Reads data from UART
  checkConnections();                // Checks and reconnects WiFi and WebSocket if necessary
  sendDistance();                    // Sends measured distances

  // Sends data every 'sendInterval' milliseconds
  if (millis() - lastSendTime > sendInterval) {
    sendDistance();
    lastSendTime = millis();
  }
}

// Callback function for WebSocket events
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_TEXT:
      Serial.printf("Data received: %s\n", payload); // Displays received data
      uart.write(payload, length);                  // Sends received data via UART
      break;
    case WStype_DISCONNECTED:
      Serial.println("Disconnected from WebSocket server!");
      deco = true; // Sets flag for reconnection
      break;
    case WStype_CONNECTED:
      Serial.println("Connected to WebSocket server!");   // Displays connection message
      webSocket.sendTXT("ESP32 connected");               // Sends connection message to server
      break;
    default:
      Serial.println("Unknown WebSocket event");          // Displays message for unknown events
      break;
  }
}

// Sends the distance measured by the ultrasonic sensors
void sendDistance() {
  unsigned int distance1 = sonar1.ping_cm(); // Measures distance in cm from the first sensor
  unsigned int distance2 = sonar2.ping_cm(); // Measures distance in cm from the second sensor

  // Checks if the measured distance is valid
  if (distance1 == 0 || distance1 > MAX_DISTANCE || distance2 == 0 || distance2 > MAX_DISTANCE) {
    return; // Skips sending if the distance is invalid
  }

  String data1 = String(distance1) + "cmA"; // Creates a string with the first distance
  String data2 = String(distance2) + "cmB"; // Creates a string with the second distance

  if (webSocket.isConnected()) {
    webSocket.sendTXT(data1);
    webSocket.sendTXT(data2);              // Sends the distances to the WebSocket server
    Serial.println("Data sent: " + data1 + " and " + data2); // Displays the sent data
  }
}

// Reads data received via UART
void readUART() {
   while (uart.available() > 0) {
    char receivedChar = uart.read(); // Reads a character from the UART port
    if (receivedChar == '\n') {      // If a newline character is received, it indicates the end of the message
      // Sends the received data to the WebSocket server
      if (uartBuffer.length() > 0) {
        webSocket.sendTXT(uartBuffer); // Sends buffered data to the WebSocket server
        Serial.println("Data sent to WebSocket server: " + uartBuffer); // Displays sent data
        uartBuffer = "";               // Resets the buffer for new data
      }
    } else {
      // Appends the character to the buffer
      uartBuffer += receivedChar;
    }
  }
}

// Connects to the WiFi network
void connectWiFi() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi!");
}

// Connects to the WebSocket server
void connectWebSocket() {
  Serial.println("Connecting to WebSocket server...");
  webSocket.begin(serverAddress, serverPort, "/websocket");
  webSocket.onEvent(webSocketEvent);
  while (!webSocket.isConnected()) {
    webSocket.loop();
    delay(1000);
    Serial.println("Connecting to WebSocket server...");
  }
  Serial.println("Connected to WebSocket server!");
}

// Checks and reconnects WiFi and WebSocket if necessary
void checkConnections() {
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }
  if (deco == true) {
    connectWebSocket();
  }
}
