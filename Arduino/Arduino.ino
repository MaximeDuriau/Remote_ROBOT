#include <Wire.h>                        // Library for I2C communication
#include <Adafruit_PWMServoDriver.h>     // Library for controlling servos with PCA9685
#include <Adafruit_MPU6050.h>            // Library for the MPU6050 sensor
#include <Adafruit_Sensor.h>             // Base library for Adafruit sensors

// Initialization of the PCA9685 servo controller
Adafruit_PWMServoDriver Moteur = Adafruit_PWMServoDriver();
// Initialization of the MPU6050 sensor
Adafruit_MPU6050 mpu;

// Structures for sensor events
sensors_event_t a, g, temp;

// Timeout for received messages
#define MESSAGE_TIMEOUT 100

// Pin definitions for DC motors
#define MOTOR1_IN1_PIN 2
#define MOTOR1_IN2_PIN 3
#define MOTOR1_PWM_PIN 5
#define MOTOR2_IN1_PIN 4
#define MOTOR2_IN2_PIN 7
#define MOTOR2_PWM_PIN 6

// Minimum and maximum servo positions
const int SERVO_MIN_POSITION[] = {80, 80, 100, 80, 80, 80};
const int SERVO_MAX_POSITION[] = {200, 540, 540, 540, 200, 500};
const int POSITION_INCREMENT = 28;  // Position increment for servos

// Pins for current and voltage measurements
const int courantMot1 = A0;
const int courantMot2 = A1;
const int tensionBat = A2;
const int courantPince = A3;

// Battery characteristics
const int ampHmax = 5.2; // Maximum battery capacity in amp-hours
const float R1 = 27000.0; // Resistor value in voltage divider
const float R2 = 7000.0;  // Resistor value in voltage divider
const float capaTotale = 624; // Total battery capacity in watt-hours

// Global variables for servo and motor control
int servoNbr = 1; // Servo number to control
int compteurVitesse = 1; // Speed counter for the motor
int vitesseMoteur = 100; // Initial motor speed
int courantPinceData = 0;
int Current_position[] = {80, 340, 380, 340, 220, 300}; // Current servo positions

// Variables for current and voltage
float courantP = 0; 
float voltage = 0; 
unsigned long lastSendTime = 0;
const unsigned long sendInterval = 500; // Interval for data sending

void setup() {
  Serial.begin(9600); // Initialize serial communication

  Moteur.begin(); // Initialize the servo controller
  Moteur.setPWMFreq(50); // Set frequency for servos
  Moteur.setPWM(5, 0, 300);
  Moteur.setPWM(4, 0, 220); // Servo initialization positions
  Moteur.setPWM(3, 0, 340); 
  Moteur.setPWM(2, 0, 380);
  Moteur.setPWM(1, 0, 340);
  Moteur.setPWM(0, 0, 80);
  initMPU(); // Initialize the MPU6050 sensor
}

void loop() {
  static String receivedMessage = ""; // Buffer for received messages
  static unsigned long messageStartTime = 0; // Start time of the message

  // Check if it's time to perform periodic tasks
  if (millis() - lastSendTime > sendInterval) {
    executePeriodicTasks();
    lastSendTime = millis();
  }

  handleSerialCommunication(receivedMessage, messageStartTime); // Handle serial communication
}

void executePeriodicTasks() {
  getVitesse(); // Retrieve and display speed
  batteryLife(); // Calculate and display battery status
  getCourantMoteur(); // Retrieve and display motor currents
  getCourantPince(); // Retrieve and display clamp current
}

void handleSerialCommunication(String &receivedMessage, unsigned long &messageStartTime) {
  while (Serial.available()) {
    char receivedChar = Serial.read(); // Read a character from serial
    receivedMessage += receivedChar; // Append the character to the received message
    messageStartTime = millis(); // Update message start time
  }

  // If the message timeout has been exceeded and a message has been received
  if (millis() - messageStartTime > MESSAGE_TIMEOUT && receivedMessage.length() > 0) {
    receivedMessage.trim(); // Remove leading and trailing spaces
    Serial.println("Message received from ESP32: " + receivedMessage); // Display the received message

    processReceivedMessage(receivedMessage); // Process the received message

    receivedMessage = ""; // Reset the received message
  }
}

void processReceivedMessage(String &message) {
  bool isNumber = true;
  for (int i = 0; i < message.length(); i++) {
    if (!isDigit(message.charAt(i))) {
      isNumber = false;
      break;
    }
  }

  if (isNumber) {
    servoNbr = message.toInt(); // If the message is a number, convert it to a servo number
  } else {
    command(message); // Otherwise, treat it as a command
  }
}
