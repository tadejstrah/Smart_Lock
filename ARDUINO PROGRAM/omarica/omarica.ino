// Program za blok 8 omaric
#include <EEPROM.h>
#include <FastCRC.h>
FastCRC8 CRC8;
#define ADDR 'a'
#define MSG_LENGTH 10 // Dolžina sporočila brez \n
#define TOGGLE 2 // DE in RE na pin 2
#define BUFLEN 10
#define BUF_OVFL_ERR 0xFE
// UKAZI //
#define NfEn 'e'
#define NfAd 'a'
#define NfRm 'r'
#define Novo 'n'
#define Open 'o'
#define OKByte 0x2A
#define REPLY 0xEE

byte temp_counter = 0;
byte resp_checksum = 0;
// Vprasanje: ADDR + sporočilo (8 bajtov) + CRC8 + \n
// Odgovor: 0xFE + checksum vprašanja + response (npr. 0x2A) + CRC8 + \n
long times[8]; // Časi odklepov vseh 8 omaric
byte buf[BUFLEN]; // Shramba dogodkov, ki se izprazni (pošlje Raspberryju) na nekaj časa. Prazni dogodki so 0. Če zmanjka prostora za nov dogodek, se zadnjo vrednost prepiše s kodo napake BUF_OVFL_ERR.
// Dogodek: 5 bitov za način odklepa, 3 biti za št. omarice (0-7)
// Načini odklepa: 1 = ključ, 11 = NFC, 111 = FP ?
void setup() {
  Serial.begin(9600);
  pinMode(TOGGLE, OUTPUT);
  memset(buf, 0, sizeof(buf));
  memset(times, 0, sizeof(times));
}

void loop() {

}

void serialEvent() {
  delay(20);
  char data[MSG_LENGTH + 1];
  if (Serial.available() == (MSG_LENGTH + 1)) {
    for (byte x = 0; x <= MSG_LENGTH; x++) {
      data[x] = Serial.read();
    }
    if (data[MSG_LENGTH] == '\n' and data[0] == ADDR) {
      char crc8 = CRC8.smbus(data, sizeof(data) - 2);
      if (crc8 == data[sizeof(data) - 2]) {
        // Checksum OK
        // Preberi vsebino sporocila --------------------------------------------
        if (temp_counter > 0 and data[1] == REPLY and data[2] == resp_checksum and data[3] == OKByte) {
          // Raspberry je uspešno prejel prvih (temp_counter) dogodkov; lahko jih izbrišemo
          for (byte x = 0; x < BUFLEN; x++) {
            if (buf[x] != 0) {
              temp_counter--;
              buf[x] = 0;
              if (temp_counter == 0) break; // Odposlani dogodki pobrisani
            }
          }
        }
        else if (data[1] == Open and data[2] - '0' < 8 and data[2] - '0' >= 0) {
          // Odklep na daljavo
          unlock(data[2] - '0');
          free(data);
          char b[1] = {OKByte};
          sendResponse(crc8, b, 1);
        }
        else if (data[1] == Novo) {
          // Kaj je novega?
          byte b[MSG_LENGTH - 3];
          memset(b, 0, sizeof(b));
          byte count = 0;
          b[0] = 0; // Ne bom več poslal ničesar
          temp_counter = 0;
          for (byte x = 0; x < BUFLEN; x++) {
            if (buf[x] != 0 and count < MSG_LENGTH - 3) {
              b[count + 1] = buf[x];
              count++;
              temp_counter++;
              if (count >= MSG_LENGTH - 4) {
                b[0] = 1; // Poslal bom še
                break;
              }
            }
          }
          sendResponse(crc8, b, sizeof(b));
        }
        // Preberi vsebino sporocila --------------------------------------------
      }
    }
  }
  else {
    while (Serial.available()) Serial.read();
  }
}
void sendResponse(const char * crc8, const char response[], const char len) {
  if (len > MSG_LENGTH - 3) {
    return; // za vsak slučaj
  }
  char data[MSG_LENGTH + 1];
  memset(data, 255, sizeof(data));
  data[0] = REPLY;
  data[1] = crc8;
  for (byte x = 0; x < len; x++)  {
    data[x + 2] = response[x];
  }
  data[MSG_LENGTH - 1] = CRC8.smbus(data, sizeof(data) - 2);
  resp_checksum = CRC8.smbus(data, sizeof(data) - 2);
  data[MSG_LENGTH] = '\n';
  digitalWrite(TOGGLE, HIGH);
  for (byte x = 0; x <= MSG_LENGTH; x++)  {
    Serial.write(data[x]);
  }
  Serial.flush();
  digitalWrite(TOGGLE, LOW);
}

void unlock(byte num) {
  // Odkleni omarico 0-7 (servo.write)
}

void openEvent(byte num) {
  // Funkcija se pokliče, ko se omarica odpre
  if (num > 7) return;
  if (abs(millis() - times[num]) > 1000) {
    // Odklep ni bil napovedan
    addEvent(0b00001000 | num);
  }
}

void addEvent(byte val) {
  // Dodaj dogodek v buffer
  for (byte x = 0; x < sizeof(buf); x++) {
    if (buf[x] == 0) {
      buf[x] = val;
      break;
    }
    else if (x == sizeof(buf) - 1 and buf[x] != 0) {
      buf[x] = BUF_OVFL_ERR;
      break;
    }
  }
}

// void sendOneByteResponse(const char *crc8, char response) {
// //  if (sizeof(response) > MSG_LENGTH - 2) return; //za vsak slučaj
// char data[MSG_LENGTH + 1];
// memset(data, 255, sizeof(data));
// data[0] = 254;
// data[1] = response;
// data[2] = crc8;
// //  for (byte x = 0; x < sizeof(response); x++) {
// //  data[x + 2] = response[x];
// //}
// data[MSG_LENGTH - 1] = CRC8.smbus(data, sizeof(data) - 2);
// data[MSG_LENGTH] = '\n';
// digitalWrite(TOGGLE, HIGH);
// for (byte x = 0; x <= MSG_LENGTH; x++) {
// Serial.write(data[x]);
// }
// Serial.flush();
// digitalWrite(TOGGLE, LOW);
// }
