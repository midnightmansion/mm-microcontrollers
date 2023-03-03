#include <SPI.h>
#include <Ethernet.h>
#include <MFRC522.h>

#define SS_PIN 7
#define RST_PIN 9
#define READER_ID "VICTOR_TEST"

MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

byte nuidPICC[4];

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

IPAddress server(192, 168, 86, 21);  // numeric IP for Google (no DNS)
// char server[] = "http://192.168.86.21";    // name address for Google (using DNS)

// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 0, 177);
IPAddress myDns(192, 168, 0, 1);

EthernetClient client;

// Variables to measure the speed
unsigned long beginMicros, endMicros;
unsigned long byteCount = 0;
bool printWebData = true;  // set to false for better speed measurement

void setup() {
  // You can use Ethernet.init(pin) to configure the CS pin
  //Ethernet.init(10);  // Most Arduino shields
  //Ethernet.init(5);   // MKR ETH shield
  //Ethernet.init(0);   // Teensy 2.0
  //Ethernet.init(20);  // Teensy++ 2.0
  //Ethernet.init(15);  // ESP8266 with Adafruit Featherwing Ethernet
  //Ethernet.init(33);  // ESP32 with Adafruit Featherwing Ethernet


  SPI.begin();
  rfid.PCD_Init();
  rfid.PCD_SetAntennaGain(0xFF);
  pinMode(A5, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);

  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }

  // start the Ethernet connection:
  Serial.println("Initialize Ethernet with DHCP:");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
      while (true) {
        delay(1);  // do nothing, no point running without Ethernet hardware
      }
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
  // give the Ethernet shield a second to initialize:
  delay(1000);
  Serial.print("connecting to ");
  Serial.print(server);
  Serial.println("...");
}

void loop() {
  // if there are incoming bytes available
  // from the server, read them and print them:

  // int len = client.available();
  // if (len > 0) {
  //   byte buffer[80];
  //   if (len > 80) len = 80;
  //   client.read(buffer, len);
  //   if (printWebData) {
  //     Serial.write(buffer, len);  // show in the serial monitor (slows some boards)
  //   }
  //   byteCount = byteCount + len;
  //   Serial.println();
  // }

  int len = client.available();
  if (len > 0) {
    String req = client.readStringUntil('*');
    Serial.println(req);
    if (req.indexOf("GOOD") >= 0) {
      digitalWrite(A5, LOW);
      digitalWrite(5, HIGH);
      delay(500);
      digitalWrite(5, LOW);
    } else if (req.indexOf("BAD") >= 0) {
      digitalWrite(A5, LOW);
      digitalWrite(6, HIGH);
      delay(500);
      digitalWrite(6, LOW);
    }

    // Serial.println(req);
    client.flush(); // TODO: investigate what this does
  }

  if (!rfid.PICC_IsNewCardPresent()) {
    digitalWrite(5, LOW);
    return;
  }

  digitalWrite(5, LOW);

  if (rfid.PICC_ReadCardSerial()) {
    digitalWrite(A5, HIGH);
    // Serial.print(F("PICC type: "));
    MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
    // Serial.println(rfid.PICC_GetTypeName(piccType));

    for (byte i = 0; i < 4; i++) {
      nuidPICC[i] = rfid.uid.uidByte[i];
    }

    Serial.print("The NUID tag is: ");
    printHex(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();
    // digitalWrite(5, HIGH);

    if (client.connect(server, 3030)) {
      client.println("GET /?rfid=" + String(getHex(rfid.uid.uidByte, rfid.uid.size)) + "&id=" + READER_ID + " HTTP/1.1");
      client.println("Connection: close");
      client.println();  // end HTTP header
      Serial.println("made a request");
      Serial.println();
    } else {
      // couldn't connect to server :(
      digitalWrite(A5, LOW);
    }
  }
}

void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? "0" : "");
    Serial.print(buffer[i], HEX);
  }
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
