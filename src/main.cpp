#include <Arduino.h>

// Identify and configure the built-in LED pin based on the target board
#if defined(ARDUINO_AVR_UNO)
  #define BOARD_NAME "Arduino Uno"
  #define STATUS_LED LED_BUILTIN
#elif defined(ESP32)
  #define BOARD_NAME "ESP32 Dev Module"
  #ifndef LED_BUILTIN
    #define STATUS_LED 2 // Default built-in LED pin for most ESP32 dev boards
  #else
    #define STATUS_LED LED_BUILTIN
  #endif
#elif defined(ESP8266)
  #define BOARD_NAME "ESP8266 (NodeMCU)"
  #ifndef LED_BUILTIN
    #define STATUS_LED 2 // NodeMCU usually has the LED on pin 2 (GPIO2) or pin 16 (D0)
  #else
    #define STATUS_LED LED_BUILTIN
  #endif
#else
  #define BOARD_NAME "Unknown Board"
  #define STATUS_LED LED_BUILTIN
#endif

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("");
  Serial.println("==========================================");
  Serial.print("Board Detected: ");
  Serial.println(BOARD_NAME);
  Serial.print("LED Pin Configured: ");
  Serial.println(STATUS_LED);
  Serial.println("==========================================");

  // Initialize LED pin as an output
  pinMode(STATUS_LED, OUTPUT);
}

void loop() {
  Serial.println("LED ON");
  digitalWrite(STATUS_LED, HIGH);
  delay(1000);

  Serial.println("LED OFF");
  digitalWrite(STATUS_LED, LOW);
  delay(1000);
}
