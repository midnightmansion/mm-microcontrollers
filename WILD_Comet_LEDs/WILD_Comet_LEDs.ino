/*
Sktech to form comet like animation for LED strips around the letters WILD.
Uses the FastLED library. The code accepts an array of LEDs, number of LEDs in the array,
starting color, target color, length of comet, and the speed of animation.

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
const uint8_t cometSpeed = 50;     // Adjust the speed (higher value results in slower speed)
const uint8_t cometLength = 40;    // Adjust the length of comet
unsigned long previousMillis = 0;  // Stores last time LEDs were updated
int count = 0;                     // Stores count for incrementing up to the NUM_LEDs


void setup() {
  // Delay to power up LEDs
  delay(1000);

  // Setup LEDs
  FastLED.addLeds<NEOPIXEL, LED_PIN_W>(leds_W, NUM_LEDS_W).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<NEOPIXEL, LED_PIN_I>(leds_I, NUM_LEDS_I).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<NEOPIXEL, LED_PIN_L>(leds_L, NUM_LEDS_L).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<NEOPIXEL, LED_PIN_D>(leds_D, NUM_LEDS_D).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(255);
}

void loop() {
  comet(leds_W, NUM_LEDS_W, Purple, Black, cometSpeed, cometLength, 1);
  //comet(leds_W, NUM_LEDS_W, Pink, Black, cometLength, cometSpeed, 1);


  /* simple LED Test
  for (int ii = 0; ii < NUM_LEDS_W; ii++) {
    // fade everything out
    leds_W.fadeToBlackBy(40);

    // let's set an led value
    leds_W[ii] = Pink;
    FastLED.delay(33);
  }
  */
}

/*
comet(leds_W, NUM_LEDS_W, Purple, Black, cometSpeed, cometLength);
comet(leds_W, NUM_LEDS_W, Pink, Black, cometSpeed, cometLength);
comet(leds_I, NUM_LEDS_I, Yellow, Black, cometSpeed, cometLength);
comet(leds_L, NUM_LEDS_L, Blue, Black, cometSpeed, cometLength);
comet(leds_D, NUM_LEDS_D, Red, Black, cometSpeed, cometLength);
comet(leds_D, NUM_LEDS_D, Orange, Black, cometSpeed, cometLength);

FastLED.show();
delay(30);
}
*/

// Not working correctly
// Attempt to imitate fadeToBlackBy but with color
void fadeToColorBy(CRGB* ledArray, uint8_t numLEDs, CRGB startColor, CRGB targetColor, int fadeAmount) {
  for (int ii = 0; ii < fadeAmount; ii++) {
    float blendFactor = float(ii) / fadeAmount;
    for (int jj = 0; jj < numLEDs; jj++) {
      ledArray[jj] = blend(startColor, targetColor, blendFactor);
    }
    FastLED.show();
    delay(50);  // Adjust the delay for the desired fade speed
  }
}

// Takes an LED array, number of LEDs in said array, start color, end color, speed, and comet length
void comet(CRGB* ledStrip, uint8_t numLeds, CHSV startColor, CHSV endColor, int speed, int tailLength, int interval) {
  /*
  unsigned long currentMillis = millis();  // Get the time
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;  // Save the last time the LEDs were updated
    count = 0;                       // Reset the count to 0 after each interval
  }
  */

  // Fade the tail LEDs to black
  fadeToBlackBy(ledStrip, numLeds, tailLength);
  //fadeToColorBy(ledStrip, numLeds, startColor, endColor, tailLength);

  // Check for position of leading comet LED
  if (count < numLeds) {
    // Set LEDs with the color value
    ledStrip[count % (numLeds + 1)] = startColor;
    count++;
  } else {
    // reset animation at completion
    count = 0;
  }

  FastLED.show();
  delay(speed);  // Delay to set the speed of the animation
}
