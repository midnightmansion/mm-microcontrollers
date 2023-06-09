#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h>
#include <Adafruit_PN532.h>

#define READER_ID "DILLON_TEST"

// NeoPixels --------------------------------------------------------------------------------------
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

// Ethernet ---------------------------------------------------------------------------------------
#define ETHERNET_PIN 10
byte mac[] = { 0x8C, 0xAE, 0x4C, 0xDE, 0x95, 0xF6 };

// Server to connect to
IPAddress server(192, 168, 1, 224);

// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 0, 177);
IPAddress myDns(192, 168, 0, 1);

EthernetClient client;

// RFID -------------------------------------------------------------------------------------------
#define SCK 14
#define MOSI 15
#define MISO 8
#define PN532_SS 12
Adafruit_PN532 nfc(SCK, MISO, MOSI, PN532_SS);

#define RFID_DEBOUNCE_DURATION 1000000
unsigned long lastRfidRead;

// Functions --------------------------------------------------------------------------------------
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

void processClientData() {
  int len = client.available();
  if (len <= 0) {
    Serial.println("No client data");
    return;
  }
  Serial.println("Processing Client Data");
  String req = client.readStringUntil('*');
  if (req.indexOf("GOOD") >= 0) {
    animation = SUCCESS_ANIMATION;
  } else if (req.indexOf("BAD") >= 0) {
    animation = FAILURE_ANIMATION;
  } else {
    processingCount += 1;
  }
  processingCount -= 1;
  client.flush();  // Clears client's buffer - probably no longer necessary
}

void makeServerRequest(String tag) {
  int clientStatus = client.connect(server, 3030);
  Serial.println("clientStatus - " + String(clientStatus));
  if (clientStatus) {
    client.println("GET /?rfid=" + tag + "&id=" + READER_ID + " HTTP/1.1");
    client.println("Connection: close");
    client.println();  // end HTTP header

    Serial.print("Sent request for ");
    Serial.println(tag);
    processingCount += 1;
    animation = PROCESSING_ANIMATION;
  } else {
    Serial.print("Request failed for ");
    Serial.println(tag);
    animation = FAILURE_ANIMATION;
  }
}

void processRFIDData() {
  unsigned long current = micros();
  unsigned long delta = current - lastRfidRead;
  if (delta < RFID_DEBOUNCE_DURATION) {
    return;
  }
  boolean success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength, 100);

  if (!success) {
    Serial.println("Timed out waiting for a card");
    return;
  }

  String tag = getHex(uid, uidLength);
  Serial.println("Got Tag: " + tag);
  lastRfidRead = current;

  makeServerRequest(tag);
}

String getHex(byte *buffer, byte bufferSize) {
  String out = "";
  for (byte i = 0; i < bufferSize; i++) {
    out += buffer[i] < 0x10 ? "0" : "";
    out += String(buffer[i], HEX);
  }
  out.toUpperCase();
  return out;
}

void halt() {
  while (true) {
    delay(1);  // do nothing
  }
}

void setup() {
  // Serial Setup
  Serial.begin(115200);
  while (!Serial) {
    delay(10);  // wait for serial port to connect. Needed for native USB port only
  }

  // RFID Setup
  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.println("Didn't find PN53x board");
    halt();
  }
  Serial.print("Found chip PN5");
  Serial.println((versiondata >> 24) & 0xFF, HEX);

  Serial.print("Firmware ver. ");
  Serial.print((versiondata >> 16) & 0xFF, DEC);
  Serial.print('.');
  Serial.println((versiondata >> 8) & 0xFF, DEC);
  nfc.setPassiveActivationRetries(0xFF);

  lastRfidRead = -1;

  // give the RFID a second to initialize:
  delay(1000);

  // Ethernet Setup
  Ethernet.init(ETHERNET_PIN);
  Serial.println("Initialize Ethernet with DHCP:");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
      halt();
    }
    if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
    }
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip, myDns);
  } else {
    Serial.print("  DHCP assigned IP ");
    Serial.println(Ethernet.localIP());
  }

  // give the Ethernet a second to initialize:
  delay(1000);



  // Animation Setup
  lastAnimation = micros();
  attract_brightness = 0;
  attract_direction = 1;
  processing_index = 0;
  status_startTime = -1;
  processingCount = 0;
  animation = ATTRACT_ANIMATION;

  pixels.begin();
}

void loop() {
  processClientData();
  processRFIDData();
  playAnimation();
}
