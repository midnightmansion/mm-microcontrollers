/*
Sktech to form comet like animation for LED strips around the letters WILD.
Uses the FastLED library. The code accepts an array of LEDs, number of LEDs in the array,
starting color, target color, length of comet, the speed of animation, and a 
personal counter for each strip.

Multiple comets on the same strip require some fine tuning in the if statements
within loop. Currently set to 2 comets for letters W and D

Currently only supports the transition from start color to either Black or White.

Author: Kevin Xu
Date: August 23, 2023
*/

#include <FastLED.h>

#define LED_PIN_W 2  // Pin for 'W'
#define LED_PIN_I 3  // Pin for 'I'
#define LED_PIN_L 4  // Pin for 'L'
#define LED_PIN_D 5  // Pin for 'D'

#define NUM_LEDS_W 30  // Number of LEDs for 'W'
#define NUM_LEDS_I 10  // Number of LEDs for 'I'
#define NUM_LEDS_L 10  // Number of LEDs for 'L'
#define NUM_LEDS_D 10  // Number of LEDs for 'D'

#define NUM_LED_COMETS 6            // Number of LED strips being used
#define NUM_STRIPS_SHARED_COMETS 2  // Number of LED strips being used

CRGBArray<NUM_LEDS_W> leds_W;
CRGBArray<NUM_LEDS_I> leds_I;
CRGBArray<NUM_LEDS_L> leds_L;
CRGBArray<NUM_LEDS_D> leds_D;

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
const uint8_t cometSpeed = 0;                       // Adjust the speed (higher = slower) (try 0-10)
const uint8_t cometLength = 3;                      // Adjust the length of comet
unsigned long previousMillis = 0;                   // Stores last time LEDs were updated
int cometPositions[NUM_LED_COMETS - 1] = {};        // One position for each strip
int cometStart[NUM_STRIPS_SHARED_COMETS - 1] = {};  // Signal for when multi-comets on one strip to start

void setup() {
  // Delay to power up LEDs
  delay(1000);

  // Setup LEDs
  FastLED.addLeds<NEOPIXEL, LED_PIN_W>(leds_W, NUM_LEDS_W).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<NEOPIXEL, LED_PIN_I>(leds_I, NUM_LEDS_I).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<NEOPIXEL, LED_PIN_L>(leds_L, NUM_LEDS_L).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<NEOPIXEL, LED_PIN_D>(leds_D, NUM_LEDS_D).setCorrection(TypicalLEDStrip);

  // Clear all LEDs
  FastLED.clear();
  FastLED.show();

  FastLED.setBrightness(255);
}

void loop() {
  // ******************************************************** STRIP 1 ********************************************************
  comet(leds_W, NUM_LEDS_W, Purple, Black, cometSpeed, cometLength, 0);
  // divide by number of comets on one strip
  if (cometPositions[0] >= floor(NUM_LEDS_W / 2)) {
    cometStart[0] = 1;
  }
  if (cometStart[0]) {
    comet(leds_W, NUM_LEDS_W, Pink, Black, cometSpeed, cometLength, 1);
  }
  
  // ******************************************************** STRIP 2 ********************************************************
  comet(leds_I, NUM_LEDS_I, Yellow, Black, cometSpeed, cometLength, 2);
  
  // ******************************************************** STRIP 3 ********************************************************
  comet(leds_L, NUM_LEDS_L, Blue, Black, cometSpeed, cometLength, 3);

  // ******************************************************** STRIP 4 ********************************************************
  comet(leds_D, NUM_LEDS_D, Red, Black, cometSpeed, cometLength, 4);
  // divide by number of comets on one strip
  if (cometPositions[4] >= floor(NUM_LEDS_D / 2)) {
    cometStart[1] = 1;
  }
  if (cometStart[1]) {
    comet(leds_D, NUM_LEDS_D, Orange, Black, cometSpeed, cometLength, 5);
  }

  FastLED.show();
}

// Takes an LED array, number of LEDs in said array, start color, end color, speed, and comet length
void comet(CRGB* ledStrip, uint8_t numLeds, CHSV startColor, CHSV endColor, int speed, int tailLength, int cometNum) {
  int& cometPosition = cometPositions[cometNum];

  // Fade out the tail of the comet
  for (int ii = 0; ii < tailLength; ii++) {
    int tailPos = (cometPosition + ii) % numLeds;  // Wrap around for tail length
    ledStrip[tailPos].fadeToBlackBy(10);           // Adjust the fade amount for the comet tail
  }

  // Set the head of the comet to the chosen color without blending
  ledStrip[cometPosition] = startColor;

  // Move the comet
  cometPosition++;
  if (cometPosition >= numLeds) {
    cometPosition = 0;  // Reset the comet's position
  }

  // Set the background to the chosen color
  for (int ii = 0; ii < numLeds; ii++) {
    ledStrip[ii] = blend(ledStrip[ii], endColor, 10);  // Blend with the background color
  }

  FastLED.show();
  delay(speed);  // Adjust the delay for the desired comet speed
}
