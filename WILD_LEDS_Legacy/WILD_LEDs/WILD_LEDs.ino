/*
Sktech to form comet like animation for LED strips around the letters WILD.
Uses the FastLED library. The code accepts an array of LEDs, number of LEDs in the array,
starting color, target color, length of comet, the speed of animation, and a 
personal counter for each strip.

Multiple comets on the same strip require some fine tuning in the if statements
within loop. Currently set to 2 comets for letters W and D.
Will need to modify D to have another LED strip for inner part.

New features include pulse, which pulses a color before fading to black and
twinkle, which keeps a solid color and randomly flashes white LEDs.

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

#define NUM_LEDS_W1 50                              // Number of LEDs for 'W1'
#define NUM_LEDS_W2 48                              // Number of LEDs for 'W2'
#define NUM_LEDS_W_TOTAL NUM_LEDS_W1 + NUM_LEDS_W2  // Number of LEDs for 'W' Total
#define NUM_LEDS_I 52                               // Number of LEDs for 'I'
#define NUM_LEDS_L 100                              // Number of LEDs for 'L'
#define NUM_LEDS_D1 70                              // Number of LEDs for Outer 'D'
#define NUM_LEDS_D2 36                              // Number of LEDs for Inner 'D'

#define NUM_LED_COMETS 11           // Number of LED comets being used
#define NUM_STRIPS_SHARED_COMETS 6  // Number of LED strips being used

#define LED_STRIP_TYPE WS2811  // Type of LED Strip used (NEOPIXEL is common)

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
const uint8_t cometLength = 3;                   // Adjust the length of comet
int cometPositions[NUM_LED_COMETS] = {};         // One position for each strip
bool cometStart[NUM_STRIPS_SHARED_COMETS] = {};  // Signal for when multi-comets on one strip to start

// Array to store LED for D2 based on D1
int D1toD2Map[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
                    10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
                    20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 
                    30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
                    40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
                    50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
                    60, 61, 62 ,63, 64, 65, 66, 67, 68, 69 };

// Define pulse parameters
const uint8_t pulseSpeed = 50;     // Adjust the speed (lower value results in faster pulse)
const uint8_t pulseIntensity = 0;  // Adjust the pulse intensity
int pulseCounter = 1;
bool toggleColor = false;

// Define twinkle parameters
const int twinkleProbability = 5;  // Adjust the probability (lower value means more frequent twinkles)
const int twinkleDuration = 100;   // Adjust the duration of a twinkle (in milliseconds)
const int twinkleColor = 255;      // Brightness of the twinkles (you can adjust this)

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

  cometPositions[1] = NUM_LEDS_W_TOTAL / 2;
  //cometPositions[2] = NUM_LEDS_W_TOTAL / 3;
  cometPositions[3] = NUM_LEDS_W2 / 2;
  cometPositions[7] = NUM_LEDS_D1 / 2;
  cometPositions[9] = NUM_LEDS_D2 / 2;

  // Clear all LEDs
  FastLED.clear();
  FastLED.show();

  FastLED.setBrightness(255);
}

void loop() {
  // ***************************************************************************************************************************************** COMET
  // ******************************************************** STRIP 1 ********************************************************
  comet(leds_W1, NUM_LEDS_W1, Purple, Black, cometSpeed, cometLength, 0, NUM_LEDS_W_TOTAL);
  comet(leds_W1, NUM_LEDS_W1, Pink, Black, cometSpeed, cometLength, 1, NUM_LEDS_W_TOTAL);

  // ******************************************************** STRIP 2 ********************************************************
  //comet(leds_W2, NUM_LEDS_W1, Purple, Black, cometSpeed, cometLength, 2, NUM_LEDS_W_TOTAL);
  if (cometPositions[0] > NUM_LEDS_W1) {
    comet(leds_W2, NUM_LEDS_W2, Purple, Black, cometSpeed, cometLength, 2, NUM_LEDS_W2);
  } else {
    fadeToBlackBy(leds_W2, NUM_LEDS_W2, 10);
  }
  if (cometPositions[1] > NUM_LEDS_W1) {
    comet(leds_W2, NUM_LEDS_W2, Yellow, Black, cometSpeed, cometLength, 3, NUM_LEDS_W2);
  } else {
    fadeToBlackBy(leds_W2, NUM_LEDS_W2, 10);
  }

  // ******************************************************** STRIP 3 ********************************************************
  comet(leds_I, NUM_LEDS_I, Yellow, Black, cometSpeed, cometLength, 4, NUM_LEDS_I);

  // ******************************************************** STRIP 4 ********************************************************
  comet(leds_L, NUM_LEDS_L, Blue, Black, cometSpeed, cometLength, 5, NUM_LEDS_L);

  // ******************************************************** STRIP 5 ********************************************************
  comet(leds_D1, NUM_LEDS_D1, Red, Black, cometSpeed, cometLength, 6, NUM_LEDS_D1);
  comet(leds_D1, NUM_LEDS_D1, Orange, Black, cometSpeed, cometLength, 7, NUM_LEDS_D1);

  // ******************************************************** STRIP 6 ********************************************************
  //cometPositions[8] = D1toD2Map[cometPositions[6]];
  //cometPositions[9] = D1toD2Map[cometPositions[7]];

  comet(leds_D2, NUM_LEDS_D2, Red, Black, cometSpeed, cometLength, 8, NUM_LEDS_D2);
  comet(leds_D2, NUM_LEDS_D2, Orange, Black, cometSpeed, cometLength, 9, NUM_LEDS_D2);


  // Move position of comet head for all comets
  for (int ii = 0; ii < NUM_LED_COMETS; ii++) {
    cometPositions[ii]++;
  }

  // ***************************************************************************************************************************************** PULSE
  /*
  if (pulseCounter % (2 * pulseSpeed) == 0) {
    toggleColor = !toggleColor;
  }

  // Increment the pulse counter
  pulseCounter++;

  // Call the pulseLights function with the updated counter for each LED strip
  pulse(leds_W, NUM_LEDS_W, toggleColor ? Purple : Pink, Black, pulseSpeed, pulseCounter);
  pulse(leds_I, NUM_LEDS_I, Yellow, Black, pulseSpeed, pulseCounter);
  pulse(leds_L, NUM_LEDS_L, Blue, Black, pulseSpeed, pulseCounter);
  pulse(leds_D1, NUM_LEDS_D1, toggleColor ? Red : Orange, Black, pulseSpeed, pulseCounter);
  pulse(leds_D2, NUM_LEDS_D2, toggleColor ? Red : Orange, Black, pulseSpeed, pulseCounter);
  */

  // ***************************************************************************************************************************************** TWINKLE
  /*
  // Twinkle white leds randomly. May need to set main color brightness down to better see twinkle (maybe ~64?)
  twinkle(leds_W, NUM_LEDS_W, Purple);
  twinkle(leds_I, NUM_LEDS_I, Yellow);
  twinkle(leds_L, NUM_LEDS_L, Blue);
  twinkle(leds_D1, NUM_LEDS_D1, Red);
  twinkle(leds_D2, NUM_LEDS_D2, Red);
  */

  //delay(10);
  FastLED.show();
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

  // Set the background to the chosen color
  for (int ii = 0; ii < numLeds; ii++) {
    ledStrip[ii] = blend(ledStrip[ii], endColor, 10);  // Blend with the background color
  }

  // Comment line below if using endColor other than black
  fadeToBlackBy(ledStrip, numLeds, 10);

  //FastLED.show();
  delay(speed);  // Adjust the delay for the desired comet speed
}

// Pulse the lights with a background color using CHSV
void pulse(CRGB* ledStrip, uint8_t numLeds, CHSV mainColor, CHSV backgroundColor, int speed, int counter) {
  // Calculate the pulse intensity based on the counter
  int pulseIntensityValue = map(counter % (2 * speed), 0, speed, 0, 255);
  if (pulseIntensityValue > 255 - pulseIntensity) {
    pulseIntensityValue = 255 - pulseIntensityValue;
  }

  CHSV color = CHSV(
    mainColor.h,
    mainColor.s,
    pulseIntensityValue);

  fill_solid(ledStrip, numLeds, color);
}

// Twinkle white LEDs randomly
void twinkle(CRGB* ledStrip, uint8_t numLeds, CHSV mainColor) {
  if (random(0, twinkleProbability) == 0) {
    int pixelIndex = random(numLeds);
    ledStrip[pixelIndex] = CHSV(mainColor.h, mainColor.s, twinkleColor);
    FastLED.show();
    delay(twinkleDuration);
    ledStrip[pixelIndex] = mainColor;  // Restore the main color
  }
}