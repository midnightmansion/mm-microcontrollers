/*
Arduino Relay Control with Pushbutton

This Arduino code controls a relay using a pushbutton. 
When the pushbutton is pressed, the relay briefly connects for a split second and then disconnects.

Components:
- Arduino board
- Pushbutton (connected to pin 2)
- Relay module (connected to pin 13)

Author: Kevin Xu
Date: August 23, 2023
*/

const int buttonPin = 2;  // Pin number for the pushbutton
const int relayPin = 13;  // Pin number for the relay

int buttonState = LOW;               // Current state of the button
int lastButtonState = LOW;           // Previous state of the button
unsigned long lastDebounceTime = 0;  // Last time the button was pressed
unsigned long debounceDelay = 50;    // Debounce time in milliseconds
boolean relayState = LOW;            // Relay state (off by default)
int flashTime = 100;           // Relay connection time in milleseconds

void setup() {
  pinMode(buttonPin, INPUT);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, relayState);  // Initialize the relay in the off state
}

void loop() {
  int reading = digitalRead(buttonPin);  // Read the state of the button

  // Check for button press with debouncing
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      // If the button is pressed, toggle the relay state briefly
      if (buttonState == HIGH) {
        relayState = HIGH;
        digitalWrite(relayPin, relayState);
        delay(flashTime);  // Adjust this value to control the relay activation time
        relayState = LOW;
        digitalWrite(relayPin, relayState);
      }
    }
  }

  lastButtonState = reading;
}
