/*
Sktech to form comet like animation for LED strips around the letters WILD.
Uses the FastLED library. The code accepts an array of LEDs, number of LEDs in the array,
starting color, target color, length of comet, the speed of animation, and a 
personal counter for each strip, a total LED count, and direction.

Currently, the comet positions are incremented within the loop and the comet function
updates the LED strip of location and color.

Multiple comets on the same strip require some fine tuning in the if statements
within loop. Currently set to 4 comets for letters W and 2 for D.
W is a special case where there are two strips, and comet must pass through strips seamlessly.
D is a special case where the inner D is synced with the outer D.

This Arduino code also controls a relay using a pushbutton. 
When the pushbutton is pressed, the relay briefly connects for a split second and then disconnects.

Current issues and places to improve: 
Comet function only controls 1 comet
Multiple comets on one LED strip must have position be manually set
Comets continueing from a different LED strip (in this case W) must be manually started
Comet mover (increment or decrement) are located in Loop
Fade to black is located outside of comet function
cometSpeed argument in comet is bad as it is a delay that is called for every comet call
which adds up to make a really long delay for even small numbers
tailLength argument does not work properly

Author: Kevin Xu
Date: September 11, 2023
*/

#include <FastLED.h>

#define LED_PIN_W1 2     // Pin for Part 1  of 'W'
#define LED_PIN_W2 8     // Pin for Part 2 of 'W'
#define LED_PIN_I 4      // Pin for 'I'
#define LED_PIN_L 5      // Pin for 'L'
#define LED_PIN_D_OUT 6  // Pin for Outer 'D'
#define LED_PIN_D_IN 7   // Pin for Inner 'D'

#define NUM_LEDS_W1 100       // Number of LEDs for 'W1'
#define NUM_LEDS_W2 53        // Number of LEDs for 'W1'
#define NUM_LEDS_W_TOTAL 153  // Number of LEDs for 'W' Total
#define NUM_LEDS_I 52         // Number of LEDs for 'I'
#define NUM_LEDS_L 70         // Number of LEDs for 'L'
#define NUM_LEDS_D1 71        // Number of LEDs for Outer 'D'
#define NUM_LEDS_D2 36        // Number of LEDs for Inner 'D'

#define NUM_LED_COMETS 24           // Number of LED comets being used
#define NUM_STRIPS_SHARED_COMETS 6  // Number of LED strips being used

#define LED_STRIP_TYPE WS2811  // Type of LED Strip used (NEOPIXEL is common)
//#define LED_STRIP_TYPE NEOPIXEL  // Type of LED Strip used (NEOPIXEL is common)

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
bool WBothActive[] = { 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
                       1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                       1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                       1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                       1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                       1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                       1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                       1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                       1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                       1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
                       0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                       1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                       1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                       1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                       1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                       1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                       1, 1};
*/                       

const int buttonPin = 9;   // Pin number for the pushbutton
const int relayPin1 = 10;   // Pin number for the relay
const int relayPin2 = 11;  // Pin number for the relay
const int relayPin3 = 12;  // Pin number for the relay

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

  // Change comet positions for extra comets so they dont overlap
  cometPositions[1] = NUM_LEDS_W_TOTAL / 4 * 1;
  cometPositions[2] = NUM_LEDS_W_TOTAL / 4 * 2;
  cometPositions[3] = NUM_LEDS_W_TOTAL / 4 * 3;
  cometPositions[5] = NUM_LEDS_W2 / 2;
  cometPositions[7] = NUM_LEDS_W2 / 2;

  cometPositions[9] = NUM_LEDS_I / 2;

  cometPositions[11] = NUM_LEDS_L / 3;
  cometPositions[12] = NUM_LEDS_L / 3 * 2;

  cometPositions[14] = NUM_LEDS_D1 / 4 * 1;
  cometPositions[15] = NUM_LEDS_D1 / 4 * 2;
  cometPositions[16] = NUM_LEDS_D1 / 4 * 3;

  cometPositions[18] = NUM_LEDS_D2 / 4 * 2;

  // Clear all LEDs
  FastLED.clear();
  FastLED.show();

  // For pushbutton and relays
  pinMode(buttonPin, INPUT);
  pinMode(relayPin1, OUTPUT);
  pinMode(relayPin2, OUTPUT);
  pinMode(relayPin3, OUTPUT);
  digitalWrite(relayPin1, relayState);
  digitalWrite(relayPin2, relayState);
  digitalWrite(relayPin3, relayState);
}

void loop() {
  // ***************************************************************************************************************************************** COMET
  // ******************************************************** STRIP 1 ********************************************************
  comet(leds_W1, NUM_LEDS_W1, Purple, Black, cometSpeed, cometLength, 0, NUM_LEDS_W_TOTAL, 1);
  comet(leds_W1, NUM_LEDS_W1, Pink, Black, cometSpeed, cometLength, 1, NUM_LEDS_W_TOTAL, 1);
  comet(leds_W1, NUM_LEDS_W1, Purple, Black, cometSpeed, cometLength, 2, NUM_LEDS_W_TOTAL, 1);
  comet(leds_W1, NUM_LEDS_W1, Pink, Black, cometSpeed, cometLength, 3, NUM_LEDS_W_TOTAL, 1);

  // ******************************************************** STRIP 2 ********************************************************
  //comet(leds_W2, NUM_LEDS_W1, Purple, Black, cometSpeed, cometLength, 2, NUM_LEDS_W_TOTAL);
  if (cometPositions[0] > NUM_LEDS_W1) {
    comet(leds_W2, NUM_LEDS_W2, Purple, Black, cometSpeed, cometLength, 4, NUM_LEDS_W2, 1);
  }
  if (cometPositions[1] > NUM_LEDS_W1) {
    comet(leds_W2, NUM_LEDS_W2, Pink, Black, cometSpeed, cometLength, 5, NUM_LEDS_W2, 1);
  }
  if (cometPositions[2] > NUM_LEDS_W1) {
    comet(leds_W2, NUM_LEDS_W2, Purple, Black, cometSpeed, cometLength, 6, NUM_LEDS_W2, 1);
  }
  if (cometPositions[3] > NUM_LEDS_W1) {
    comet(leds_W2, NUM_LEDS_W2, Pink, Black, cometSpeed, cometLength, 7, NUM_LEDS_W2, 1);
  }

  // ******************************************************** STRIP 3 ********************************************************
  comet(leds_I, NUM_LEDS_I, Yellow, Black, cometSpeed, cometLength, 8, NUM_LEDS_I, 1);
  comet(leds_I, NUM_LEDS_I, Yellow, Black, cometSpeed, cometLength, 9, NUM_LEDS_I, 1);

  // ******************************************************** STRIP 4 ********************************************************
  comet(leds_L, NUM_LEDS_L, Blue, Black, cometSpeed, cometLength, 10, NUM_LEDS_L, 1);
  comet(leds_L, NUM_LEDS_L, Blue, Black, cometSpeed, cometLength, 11, NUM_LEDS_L, 1);
  comet(leds_L, NUM_LEDS_L, Blue, Black, cometSpeed, cometLength, 12, NUM_LEDS_L, 1);

  // ******************************************************** STRIP 5 ********************************************************
  comet(leds_D1, NUM_LEDS_D1, Red, Black, cometSpeed, cometLength, 13, NUM_LEDS_D1, 1);
  comet(leds_D1, NUM_LEDS_D1, Orange, Black, cometSpeed, cometLength, 14, NUM_LEDS_D1, 1);
  comet(leds_D1, NUM_LEDS_D1, Red, Black, cometSpeed, cometLength, 15, NUM_LEDS_D1, 1);
  comet(leds_D1, NUM_LEDS_D1, Orange, Black, cometSpeed, cometLength, 16, NUM_LEDS_D1, 1);

  // ******************************************************** STRIP 6 ********************************************************
  // Move position of comet head for all comets before strip 6 as it is synced with strip 5
  for (int ii = 0; ii < 17; ii++) {
    cometPositions[ii]++;
  }

  // W is a special case with two strips together with some LEDs that will never be active
  for (int ii = 0; ii < 8; ii++) {
    //moveActiveLED(WBothActive, cometPositions[ii], NUM_LEDS_W_TOTAL);
  }

  // Inner D comets
  comet(leds_D2, NUM_LEDS_D2, Orange, Black, cometSpeed, cometLength, 17, NUM_LEDS_D2, 0);
  comet(leds_D2, NUM_LEDS_D2, Red, Black, cometSpeed, cometLength, 18, NUM_LEDS_D2, 0);

  // Inner D LED strip is backwards compared to all other letters, and must go backwards
  for (int ii = 17; ii < 21; ii++) {
    cometPositions[ii]--;
  }

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
void comet(CRGB* ledStrip, uint8_t numLeds, CHSV startColor, CHSV endColor, int speed, int tailLength, int cometNum, int totalLed, bool direction) {
  int& cometPosition = cometPositions[cometNum];

  // Fade out the tail of the comet
  for (int ii = 0; ii < tailLength; ii++) {
    int tailPos = (cometPosition + ii) % numLeds;  // Wrap around for tail length
    ledStrip[tailPos].fadeToBlackBy(10);           // Adjust the fade amount for the comet tail
  }

  // Ensures the comet head isn't blended by the endColor
  if (cometPosition < totalLed) {
    // Set the head of the comet to the chosen color without blending
    ledStrip[cometPosition] = startColor;
  }

  // Once comet has reached its end, reset
  if ((cometPosition >= totalLed) && direction) {
    cometPosition = 0;  // Reset the comet's position
  }

  // If backwards, reset at 0 instead of total LEDs
  if ((cometPosition == 0) && !direction) {
    cometPosition = numLeds;  // Reset the comet's position
  }

  // Blend only if it isn't black, as we do it in the main loop
  if (endColor != Black) {
    // Set the background to the chosen color
    for (int ii = 0; ii < numLeds; ii++) {
      ledStrip[ii] = blend(ledStrip[ii], endColor, 10);  // Blend with the background color
    }
  }

  //FastLED.show();
  delay(speed);  // Adjust the delay for the desired comet speed
}

// Moves the comet to the next active LED
void moveActiveLED(bool* activeList, int& cometPosition, int numLeds) {
  // Find the next active LED
  while (!activeList[cometPosition]) {
    cometPosition = (cometPosition + 1) % numLeds;  // Wrap around for circular animation
  }
}
