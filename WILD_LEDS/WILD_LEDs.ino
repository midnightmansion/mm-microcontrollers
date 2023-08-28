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
Date: August 25, 2023
*/

#include <FastLED.h>

#define LED_PIN_W 2  // Pin for 'W'
#define LED_PIN_I 3  // Pin for 'I'
#define LED_PIN_L 4  // Pin for 'L'
#define LED_PIN_D 5  // Pin for 'D'

#define NUM_LEDS_W 100  // Number of LEDs for 'W'
#define NUM_LEDS_I 100  // Number of LEDs for 'I'
#define NUM_LEDS_L 100  // Number of LEDs for 'L'
#define NUM_LEDS_D 100  // Number of LEDs for 'D'

#define NUM_LED_COMETS 6            // Number of LED strips being used
#define NUM_STRIPS_SHARED_COMETS 2  // Number of LED strips being used

#define LED_STRIP_TYPE WS2811  // Type of LED Strip used (NEOPIXEL is common)

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
const uint8_t cometSpeed = 0;                   // Adjust the speed (higher = slower) (try 0-10)
const uint8_t cometLength = 3;                  // Adjust the length of comet
int cometPositions[NUM_LED_COMETS] = {};        // One position for each strip
int cometStart[NUM_STRIPS_SHARED_COMETS] = {};  // Signal for when multi-comets on one strip to start

// Define pulse parameters
const uint8_t pulseSpeed = 50;     // Adjust the speed (lower value results in faster pulse)
const uint8_t pulseIntensity = 0;  // Adjust the pulse intensity
int pulseCounter = 1;
bool toggleColor = false;

// Define twinkle parameters
const int twinkleProbability = 5;  // Adjust the probability (lower value means more frequent twinkles)
const int twinkleDuration = 100;   // Adjust the duration of a twinkle (in milliseconds)
const int twinkleColor = 255;       // Brightness of the twinkles (you can adjust this)


void setup() {
  // Delay to power up LEDs
  delay(1000);

  // Setup LEDs
  FastLED.addLeds<LED_STRIP_TYPE, LED_PIN_W>(leds_W, NUM_LEDS_W).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_STRIP_TYPE, LED_PIN_I>(leds_I, NUM_LEDS_I).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_STRIP_TYPE, LED_PIN_L>(leds_L, NUM_LEDS_L).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_STRIP_TYPE, LED_PIN_D>(leds_D, NUM_LEDS_D).setCorrection(TypicalLEDStrip);

  // Clear all LEDs
  FastLED.clear();
  FastLED.show();

  FastLED.setBrightness(255);
}

void loop() {
  /*
  // ***************************************************************************************************************************************** COMET
  // ******************************************************** STRIP 1 ********************************************************
  comet(leds_W, NUM_LEDS_W, Purple, Black, cometSpeed, cometLength, 0);
  // When first comet reaches halfway start next comet (divide by number of comets on one strip)
  if (cometPositions[0] >= floor(NUM_LEDS_W / 2)) {
    cometStart[0] = 1;
  }
  if (cometStart[0]) {
    comet(leds_W, NUM_LEDS_W, Pink, Black , cometSpeed, cometLength, 1);
  }
  
  // ******************************************************** STRIP 2 ********************************************************
  comet(leds_I, NUM_LEDS_I, Yellow, White, cometSpeed, cometLength, 2);
  
  // ******************************************************** STRIP 3 ******************************************************** 
  comet(leds_L, NUM_LEDS_L, Blue, White, cometSpeed, cometLength, 3);

  // ******************************************************** STRIP 4 ********************************************************
  comet(leds_D, NUM_LEDS_D, Red, White, cometSpeed, cometLength, 4);
  // When first comet reaches halfway start next comet (divide by number of comets on one strip)
  if (cometPositions[4] >= floor(NUM_LEDS_D / 2)) {
    cometStart[1] = 1;
  }
  if (cometStart[1]) {
    comet(leds_D, NUM_LEDS_D, Orange, White, cometSpeed, cometLength, 5);
  }
  */

  // ***************************************************************************************************************************************** PULSE
  if (pulseCounter % (2 * pulseSpeed) == 0) {
    toggleColor = !toggleColor;
  }

  // Increment the pulse counter
  pulseCounter++;

  // Call the pulseLights function with the updated counter for each LED strip
  pulse(leds_W, NUM_LEDS_W, toggleColor ? Purple : Pink, Black, pulseSpeed, pulseCounter);
  pulse(leds_I, NUM_LEDS_I, Yellow, Black, pulseSpeed, pulseCounter);
  pulse(leds_L, NUM_LEDS_L, Blue, Black, pulseSpeed, pulseCounter);
  pulse(leds_D, NUM_LEDS_D, toggleColor ? Red : Orange, Black, pulseSpeed, pulseCounter);

  // ***************************************************************************************************************************************** TWINKLE
  /*
  // Twinkle white leds randomly. May need to set main color brightness down to better see twinkle (maybe ~64?)
  twinkle(leds_W, NUM_LEDS_W, Purple);
  twinkle(leds_I, NUM_LEDS_I, Yellow);
  twinkle(leds_L, NUM_LEDS_L, Blue);
  checkForTwinkles(leds_D, NUM_LEDS_D, Red);
  */

  FastLED.show();
}

// Takes an LED array, number of LEDs in said array, start color, end color, speed, and comet length, and comet identifier number
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