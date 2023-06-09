#include <Adafruit_NeoPixel.h>
#define PIXEL_PIN 16
#define NUM_PIXELS 16
Adafruit_NeoPixel pixels(NUM_PIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);


unsigned long lastAnimation;
#define ATTRACT_ANIMATION 0
#define ATTRACT_DURATION 1000000  // Microseconds
#define MIN_ATTRACT_BRIGHTNESS 10
#define MAX_ATTRACT_BRIGHTNESS 200
int attract_brightness;
int attract_direction;
#define PROCESSING_ANIMATION 1
#define PROCESSING_VELOCITY 16000  // 1 pixel per N microseconds
int processing_index;
#define SUCCESS_ANIMATION 2
#define FAILURE_ANIMATION 3
#define STATUS_DURATION 1000000  // microseconds
unsigned long status_startTime;

int animation;
int processingCount;

void playAnimation() {
  unsigned long currentTime = micros();
  unsigned long timeElapsed = currentTime - lastAnimation;
  switch (animation) {
    case ATTRACT_ANIMATION:
    {
      long delta = timeElapsed * MAX_ATTRACT_BRIGHTNESS / ATTRACT_DURATION;
      if (delta < 1) {
        break;
      }
      int newBrightness = attract_brightness + delta * attract_direction;
      if (newBrightness < MIN_ATTRACT_BRIGHTNESS) {
        newBrightness = 2 * MIN_ATTRACT_BRIGHTNESS - newBrightness;
        attract_direction = 1;
      }
      if (MAX_ATTRACT_BRIGHTNESS < newBrightness) {
        newBrightness = 2 * MAX_ATTRACT_BRIGHTNESS - newBrightness;
        attract_direction = -1;
      }
      attract_brightness = newBrightness;
      for (int i = 0; i < NUM_PIXELS; i++) {
        pixels.setPixelColor(i, pixels.Color(0, 0, attract_brightness));
      }
      pixels.show();
      lastAnimation = currentTime;
      break;
    }
    case PROCESSING_ANIMATION:
    {
      long delta = timeElapsed / PROCESSING_VELOCITY;
      if (delta < 1) {
        break;
      }
      pixels.setPixelColor(processing_index, pixels.Color(0, 0, 0));
      processing_index = (processing_index + delta) % NUM_PIXELS;
      pixels.setPixelColor(processing_index, pixels.Color(200, 200, 0));
      pixels.show();
      lastAnimation = currentTime;
      break;
    }
    case SUCCESS_ANIMATION:
    case FAILURE_ANIMATION:
    {
      if (status_startTime == -1) {
        status_startTime = currentTime;
        for (int i = 0; i < NUM_PIXELS; i++) {
          if (animation == SUCCESS_ANIMATION) {
            pixels.setPixelColor(i, pixels.Color(0, 200, 0));
          } else {
            pixels.setPixelColor(i, pixels.Color(200, 0, 0));
          }
        }
        pixels.show();
      } else {
        long delta = currentTime - status_startTime;
        if (delta > STATUS_DURATION) {
          status_startTime = -1;
          for (int i = 0; i < NUM_PIXELS; i++) {
            pixels.setPixelColor(i, pixels.Color(0, 0, 0));
          }
          pixels.show();
          if (processingCount > 0) {
            animation = PROCESSING_ANIMATION;
          } else {
            animation = ATTRACT_ANIMATION;
          }
        }
      }
      lastAnimation = currentTime;
    }
  }
}


void setup() {
  // Setup global variables
  lastAnimation = micros();
  attract_brightness = 0;
  attract_direction = 1;
  processing_index = 0;
  status_startTime = -1;
  processingCount = 1;
  animation = ATTRACT_ANIMATION;

  pixels.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  playAnimation();
}
