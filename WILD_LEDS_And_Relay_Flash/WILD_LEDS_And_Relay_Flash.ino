/*
Sktech to form comet like animation for LED strips around the letters WILD.
Uses the FastLED library. The code accepts an array of LEDs, number of LEDs in the array,
starting color, target color, length of comet, the speed of animation, and a 
personal counter for each strip.

Multiple comets on the same strip require some fine tuning in the if statements
within loop. Currently set to 4 comets for letters W and 2 for D.

This Arduino code also controls a relay using a pushbutton. 
When the pushbutton is pressed, the relay briefly connects for a split second and then disconnects.

Author: Kevin Xu
Date: September 3, 2023
*/

#include <FastLED.h>

#define LED_PIN_W1 2     // Pin for Part 1  of 'W'
#define LED_PIN_W2 3     // Pin for Part 2 of 'W'
#define LED_PIN_I 4      // Pin for 'I'
#define LED_PIN_L 5      // Pin for 'L'
#define LED_PIN_D_OUT 6  // Pin for Outer 'D'
#define LED_PIN_D_IN 7   // Pin for Inner 'D'

#define NUM_LEDS_W1 100       // Number of LEDs for 'W1'
#define NUM_LEDS_W2 48        // Number of LEDs for 'W2'
#define NUM_LEDS_W_TOTAL 148  // Number of LEDs for 'W' Total
#define NUM_LEDS_I 52         // Number of LEDs for 'I'
#define NUM_LEDS_L 70         // Number of LEDs for 'L'
#define NUM_LEDS_D1 70        // Number of LEDs for Outer 'D'
#define NUM_LEDS_D2 36        // Number of LEDs for Inner 'D'

#define NUM_LED_COMETS 20           // Number of LED comets being used
#define NUM_STRIPS_SHARED_COMETS 6  // Number of LED strips being used

//#define LED_STRIP_TYPE WS2811  // Type of LED Strip used (NEOPIXEL is common)
#define LED_STRIP_TYPE NEOPIXEL  // Type of LED Strip used (NEOPIXEL is common)

CRGBArray<NUM_LEDS_W1> leds_W1;
CRGBArray<NUM_LEDS_W2> leds_W2;
CRGBArray<NUM_LEDS_I> leds_I;
CRGBArray<NUM_LEDS_L> leds_L;
CRGBArray<NUM_LEDS_D1> leds_D1;
CRGBArray<NUM_LEDS_D2> leds_D2;

// Define some common colors
const CHSV Black = CHSV(0, 0, 0);
const CHSV White = CHSV(0, 0, 255);
const CHSV Red = CHSV(0, 255, 255);
const CHSV Orange = CHSV(32, 255, 255);
const CHSV Yellow = CHSV(64, 255, 255);
const CHSV Green = CHSV(96, 255, 255);
const CHSV Cyan = CHSV(128, 255, 255);
const CHSV Blue = CHSV(160, 255, 255);
const CHSV Purple = CHSV(192, 255, 255);
const CHSV Pink = CHSV(224, 255, 255);

// Define comet parameters
const uint8_t cometSpeed = 0;                    // Adjust the speed (higher = slower) (try 0-10)
const uint8_t cometLength = 20;                  // Adjust the length of comet
int cometPositions[NUM_LED_COMETS] = {};         // One position for each strip
bool cometStart[NUM_STRIPS_SHARED_COMETS] = {};  // Signal for when multi-comets on one strip to start

/*
// Array to store LED positions for D2 based on D1
int D1toD2Map[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
                    10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
                    20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
                    30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
                    40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
                    50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
                    60, 61, 62, 63, 64, 65, 66, 67, 68, 69 };
*/

const int buttonPin = 8;   // Pin number for the pushbutton
const int relayPin1 = 9;   // Pin number for the relay
const int relayPin2 = 10;  // Pin number for the relay
const int relayPin3 = 11;  // Pin number for the relay


int buttonState = LOW;               // Current state of the button
int lastButtonState = LOW;           // Previous state of the button
unsigned long lastDebounceTime = 0;  // Last time the button was pressed
unsigned long debounceDelay = 50;    // Debounce time in milliseconds
boolean relayState = LOW;            // Relay state (off by default)
int flashTime = 100;                 // Relay connection time in milleseconds

void setup() {
  // Delay to power up LEDs
  delay(1000);

  // Setup LEDs
  FastLED.addLeds<LED_STRIP_TYPE, LED_PIN_W1>(leds_W1, NUM_LEDS_W1).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_STRIP_TYPE, LED_PIN_W2>(leds_W2, NUM_LEDS_W2).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_STRIP_TYPE, LED_PIN_I>(leds_I, NUM_LEDS_I).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_STRIP_TYPE, LED_PIN_L>(leds_L, NUM_LEDS_L).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_STRIP_TYPE, LED_PIN_D_OUT>(leds_D1, NUM_LEDS_D1).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_STRIP_TYPE, LED_PIN_D_IN>(leds_D2, NUM_LEDS_D2).setCorrection(TypicalLEDStrip);

  cometPositions[1] = NUM_LEDS_W_TOTAL / 4 * 1;
  cometPositions[2] = NUM_LEDS_W_TOTAL / 4 * 2;
  cometPositions[3] = NUM_LEDS_W_TOTAL / 4 * 3;
  cometPositions[5] = NUM_LEDS_W2 / 2;
  cometPositions[7] = NUM_LEDS_W2 / 2;
  cometPositions[11] = NUM_LEDS_D1 / 4 * 1;
  cometPositions[12] = NUM_LEDS_D1 / 4 * 2;
  cometPositions[13] = NUM_LEDS_D1 / 4 * 3;


  // Clear all LEDs
  FastLED.clear();
  FastLED.show();

  // For pushbutton and relays
  pinMode(buttonPin, INPUT);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin1, relayState);
  digitalWrite(relayPin2, relayState);
  digitalWrite(relayPin3, relayState);
}

void loop() {
  // ***************************************************************************************************************************************** COMET
  // ******************************************************** STRIP 1 ********************************************************
  comet(leds_W1, NUM_LEDS_W1, Purple, Black, cometSpeed, cometLength, 0, NUM_LEDS_W_TOTAL);
  comet(leds_W1, NUM_LEDS_W1, Pink, Black, cometSpeed, cometLength, 1, NUM_LEDS_W_TOTAL);
  comet(leds_W1, NUM_LEDS_W1, Purple, Black, cometSpeed, cometLength, 2, NUM_LEDS_W_TOTAL);
  comet(leds_W1, NUM_LEDS_W1, Pink, Black, cometSpeed, cometLength, 3, NUM_LEDS_W_TOTAL);

  // ******************************************************** STRIP 2 ********************************************************
  //comet(leds_W2, NUM_LEDS_W1, Purple, Black, cometSpeed, cometLength, 2, NUM_LEDS_W_TOTAL);
  if (cometPositions[0] > NUM_LEDS_W1) {
    comet(leds_W2, NUM_LEDS_W2, Purple, Black, cometSpeed, cometLength, 4, NUM_LEDS_W2);
  }
  if (cometPositions[1] > NUM_LEDS_W1) {
    comet(leds_W2, NUM_LEDS_W2, Pink, Black, cometSpeed, cometLength, 5, NUM_LEDS_W2);
  }
  if (cometPositions[2] > NUM_LEDS_W1) {
    comet(leds_W2, NUM_LEDS_W2, Purple, Black, cometSpeed, cometLength, 6, NUM_LEDS_W2);
  }
  if (cometPositions[3] > NUM_LEDS_W1) {
    comet(leds_W2, NUM_LEDS_W2, Pink, Black, cometSpeed, cometLength, 7, NUM_LEDS_W2);
  }

  // ******************************************************** STRIP 3 ********************************************************
  comet(leds_I, NUM_LEDS_I, Yellow, Black, cometSpeed, cometLength, 8, NUM_LEDS_I);

  // ******************************************************** STRIP 4 ********************************************************
  comet(leds_L, NUM_LEDS_L, Blue, Black, cometSpeed, cometLength, 9, NUM_LEDS_L);

  // ******************************************************** STRIP 5 ********************************************************
  comet(leds_D1, NUM_LEDS_D1, Red, Black, cometSpeed, cometLength, 10, NUM_LEDS_D1);
  comet(leds_D1, NUM_LEDS_D1, Orange, Black, cometSpeed, cometLength, 11, NUM_LEDS_D1);
  comet(leds_D1, NUM_LEDS_D1, Red, Black, cometSpeed, cometLength, 12, NUM_LEDS_D1);
  comet(leds_D1, NUM_LEDS_D1, Orange, Black, cometSpeed, cometLength, 13, NUM_LEDS_D1);

  // ******************************************************** STRIP 6 ********************************************************
  // Move position of comet head for all comets before strip 6 as it is synced with strip 5
  for (int ii = 0; ii < NUM_LED_COMETS; ii++) {
    cometPositions[ii]++;
  }

  cometPositions[14] = cometPositions[10] / 2;
  cometPositions[15] = cometPositions[11] / 2;
  cometPositions[16] = cometPositions[12] / 2;
  cometPositions[17] = cometPositions[13] / 2;

  comet(leds_D2, NUM_LEDS_D2, Red, Black, cometSpeed, cometLength, 14, NUM_LEDS_D2);
  comet(leds_D2, NUM_LEDS_D2, Orange, Black, cometSpeed, cometLength, 15, NUM_LEDS_D2);
  comet(leds_D2, NUM_LEDS_D2, Red, Black, cometSpeed, cometLength, 16, NUM_LEDS_D2);
  comet(leds_D2, NUM_LEDS_D2, Orange, Black, cometSpeed, cometLength, 17, NUM_LEDS_D2);

  // Fade out all comets here rather than in comet function
  fadeToBlackBy(leds_W1, NUM_LEDS_W1, cometLength);
  fadeToBlackBy(leds_W2, NUM_LEDS_W2, cometLength);
  fadeToBlackBy(leds_I, NUM_LEDS_I, cometLength);
  fadeToBlackBy(leds_L, NUM_LEDS_L, cometLength);
  fadeToBlackBy(leds_D1, NUM_LEDS_D1, cometLength);
  fadeToBlackBy(leds_D2, NUM_LEDS_D2, cometLength);

  FastLED.show();


  // Relay Code
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
        digitalWrite(relayPin1, relayState);
        digitalWrite(relayPin2, relayState);
        digitalWrite(relayPin3, relayState);

        delay(flashTime);  // Adjust this value to control the relay activation time
        relayState = LOW;
        digitalWrite(relayPin1, relayState);
        digitalWrite(relayPin2, relayState);
        digitalWrite(relayPin3, relayState);
      }
    }
  }

  lastButtonState = reading;
}

// Takes an LED array, number of LEDs in said array, start color, end color, speed, and comet length, and comet identifier number
void comet(CRGB* ledStrip, uint8_t numLeds, CHSV startColor, CHSV endColor, int speed, int tailLength, int cometNum, int totalLed) {
  int& cometPosition = cometPositions[cometNum];

  // Fade out the tail of the comet
  for (int ii = 0; ii < tailLength; ii++) {
    int tailPos = (cometPosition + ii) % numLeds;  // Wrap around for tail length
    ledStrip[tailPos].fadeToBlackBy(10);           // Adjust the fade amount for the comet tail
  }

  if (cometPosition < totalLed) {
    // Set the head of the comet to the chosen color without blending
    ledStrip[cometPosition] = startColor;
  }

  // Move the comet
  //cometPosition++;
  if (cometPosition >= totalLed) {
    cometPosition = 0;  // Reset the comet's position
  }

  if (endColor != Black) {
    // Set the background to the chosen color
    for (int ii = 0; ii < numLeds; ii++) {
      ledStrip[ii] = blend(ledStrip[ii], endColor, 10);  // Blend with the background color
    }
  }

  //FastLED.show();
  delay(speed);  // Adjust the delay for the desired comet speed
}