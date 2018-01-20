// Program za blok 8 omaric
#include <EEPROM.h>
#include <FastCRC.h>
FastCRC8 CRC8;

#define ADDR 'a'
#define MSG_LENGTH 10 // Dolžina sporočila brez \n
#define TOGGLE 2 //DE in RE na pin 2

// UKAZI //
#define NfEn 'e'
#define NfAd 'a'
#define NfRm 'r'
#define Novo 'n'
#define Open 'o'
#define OKByte 0x2A
#define REPLY 0xFE

// Vprasanje: ADDR + sporočilo (8 bajtov) + CRC8 + \n
// Odgovor: 0xFE + checksum vprašanja + response (npr. 0x2A) + CRC8 + \n


char buf[10];

void setup() {
  Serial.begin(9600);
  pinMode(TOGGLE, OUTPUT);
  memset(buf, 255, sizeof(buf));
}

void loop() {
  if (Serial.available()) {
    delay(20);
    char data[MSG_LENGTH + 1];
    if (Serial.available() == MSG_LENGTH + 1) {
      for (byte x = 0; x <= MSG_LENGTH; x++) {
        data[x] = Serial.read();
      }
      if (data[MSG_LENGTH] == '\n' and data[0] == ADDR) {
        char crc8 = CRC8.smbus(data, sizeof(data) - 2);
        if (crc8 == data[sizeof(data) - 2]) {
          // Checksum OK
          // Preberi vsebino sporocila --------------------------------------------
          if (data[1] == Open and data[2] - '0' < 8 and data[2] - '0' >= 0) {
            // Odklep na daljavo
            unlock(data[2] - '0');
            free(data);
            char b[1] = {OKByte};
            sendResponse(crc8, b, 1);
          }
          else if (data[1] == Novo) {
            char b[8];
            EEPROM.write(10,0xAF);
            for(byte x=0;x<8;x++) {
              b[x] = EEPROM.read(x+10);
            }
            sendResponse(crc8,b,8);
          }

          // Preberi vsebino sporocila --------------------------------------------
        }
      }
    }
    else {
      while (Serial.available()) Serial.read();
    }
  }
}


void sendResponse(const char *crc8, const char response[], const char len) {
  if (len > MSG_LENGTH - 2) return; //za vsak slučaj
  char data[MSG_LENGTH + 1];
  memset(data, 255, sizeof(data));
  data[0] = REPLY;
  data[1] = crc8;
  for (byte x = 0; x < len; x++) {
    data[x + 2] = response[x];
  }
  data[MSG_LENGTH - 1] = CRC8.smbus(data, sizeof(data) - 2);
  data[MSG_LENGTH] = '\n';
  digitalWrite(TOGGLE, HIGH);
  for (byte x = 0; x <= MSG_LENGTH; x++) {
    Serial.write(data[x]);
  }
  Serial.flush();
  digitalWrite(TOGGLE, LOW);
}

void unlock(byte num) {
  //odkleni omarico 0-7 (servo.write)
}



//void sendOneByteResponse(const char *crc8, char response) {
//  //  if (sizeof(response) > MSG_LENGTH - 2) return; //za vsak slučaj
//  char data[MSG_LENGTH + 1];
//  memset(data, 255, sizeof(data));
//  data[0] = 254;
//  data[1] = response;
//  data[2] = crc8;
//  //  for (byte x = 0; x < sizeof(response); x++) {
//  //  data[x + 2] = response[x];
//  //}
//  data[MSG_LENGTH - 1] = CRC8.smbus(data, sizeof(data) - 2);
//  data[MSG_LENGTH] = '\n';
//  digitalWrite(TOGGLE, HIGH);
//  for (byte x = 0; x <= MSG_LENGTH; x++) {
//    Serial.write(data[x]);
//  }
//  Serial.flush();
//  digitalWrite(TOGGLE, LOW);
//}
