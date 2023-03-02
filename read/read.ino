#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9

MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

byte nuidPICC[4];

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  rfid.PCD_SetAntennaGain(0xFF);
  pinMode(5, OUTPUT);
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent()) {
    digitalWrite(5, LOW);
    return;
  }

  if (!rfid.PICC_ReadCardSerial()) {
    digitalWrite(5, LOW);
    return;
  }

  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));

  for (byte i = 0; i < 4; i++) {
    nuidPICC[i] = rfid.uid.uidByte[i];
  }

  Serial.print("The NUID tag is: ");
  printHex(rfid.uid.uidByte, rfid.uid.size);
  digitalWrite(5, HIGH);
  Serial.println();
}

void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? "0" : "");
    Serial.print(buffer[i], HEX);
  }
}