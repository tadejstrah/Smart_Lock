// Program za blok 8 omaric
#include <EEPROM.h>
#include <FastCRC.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include <Servo.h>
#include <Adafruit_Fingerprint.h>

Servo servo0;
PN532_I2C pn532i2c(Wire);
PN532 nfc(pn532i2c);
//SoftwareSerial softserial(6,7); //RX, TX
FastCRC8 CRC8;

#define ADDR 1
#define MSG_LENGTH 11 // Dolžina sporočila brez \n
#define TOGGLE 8  // DE in RE na pin 2
#define BUFLEN 15
#define BUF_OVFL_ERR 0xFE
#define NFC_DUPLICATE_ERR 0xFE
#define LOCK 50
#define UNLOCK 100
#define serial Serial
#define FP_RX 2
#define FP_TX 3
// UKAZI //
#define NfEn 0xA1
#define NfAd 0xA2
#define NfRm 0xA3
#define Novo 0xDA
#define Open 0xCD
#define FLock 0xCF
#define OKByte 0x2A
#define REPLY 0xEE
SoftwareSerial fp_serial(FP_RX,FP_TX);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&fp_serial);
long task2millis = 0;
byte temp_counter = 0;
// Vprasanje: ADDR + sporočilo (8 bajtov) + CRC8 + \n
// Odgovor: 0xFE + checksum vprašanja + response (npr. 0x2A) + CRC8 + \n
long task1millis = 0;
boolean nfc_works = 1;
long times[8]; // Časi odklepov vseh 8 omaric
byte waiting = 0;
byte prevStates = 255;
byte buf[BUFLEN]; // Shramba dogodkov, ki se izprazni (pošlje Raspberryju) na nekaj časa. Prazni dogodki so 0. Če zmanjka prostora za nov dogodek, se zadnjo vrednost prepiše s kodo napake BUF_OVFL_ERR.
// Dogodek: 5 bitov za način odklepa, 3 biti za št. omarice (0-7)
// Načini odklepa: 1 = ključ, 2 = NFC, 3 = zaklep
void setup() {
  servo0.attach(10);
  Serial.begin(9600);
  serial.begin(9600);
  pinMode(TOGGLE, OUTPUT);
  digitalWrite(TOGGLE,LOW);
  memset(buf, 0, sizeof(buf));
  memset(times, 0, sizeof(times));
  nfc.begin();
  delay(50);
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN532 board");
    nfc_works = 0;
  }
  nfc.setPassiveActivationRetries(0xFF);
  nfc.SAMConfig();
  Serial.println("Waiting for card");
  finger.begin(57600);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
  
  }

  finger.getTemplateCount();
  Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
  Serial.println("Waiting for valid finger...");
}

void loop() {

  // Card?
  if(nfc_works == 1) {
    boolean success;
    uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
    uint8_t uidLength;
    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength,10);
    if (success) {
      Serial.print("UID Value: ");
      for (uint8_t i = 0; i < uidLength; i++)
      {
        Serial.print(" 0x"); Serial.print(uid[i], HEX);
      }
      Serial.println("");
      byte tmp = findByUID(uid);
      Serial.println(tmp);
      if(0) {
        // Rdeča luč al neki
      }
      else {
        // Zelena luč al neki
        unlock(0);
        addEvent(0b00010000 | tmp);
        times[tmp] = millis();
      }
      delay(500);
    }
  }
  if(serial.available()) {
    serialCheck();
    Serial.println("available");
  }
  if(Serial.available()) {
    char c = Serial.read();
    if(c=='e') {
      // Dump EEPROM
      for(byte x=0;x<50;x++) {
        Serial.print(x); Serial.print("  ");
        Serial.println(EEPROM.read(x));
      }
    }
  }
  if(abs(task1millis - millis()) > 500) {
    task1millis = millis();
    for (byte x=0;x<1;x++) {
      if((1 & (waiting >> x)) and abs(millis()-times[x]) > 2000) { // ali čaka na zaklep IN je minilo od tega dogodka vec kot 2s ?
        // Zakleni omarico
        Serial.print("locked ");Serial.println(x);
        lock(x);
        addEvent(0b00011000 | x);
        waiting &= ~(1 << x);
      }
      else if ((1 & (waiting >> x))) {
        // Ali samo čaka na zaklep?
        // Pisk
      }
    }
  }

  if(abs(task2millis-millis()) > 100) {
    task2millis = millis();
    if(getFingerprintIDez() != -1){
      unlock(0);
    }
  }
  byte currentStates = getStates();
  for(byte x=0;x<1;x++) {
    if(!(1 & (prevStates >> x)) and (1 & (currentStates >> x))) {
      // X se je zaprla
      closeEvent(x);
    }
    else if((1 & (prevStates >> x)) and (!(1 & (currentStates >> x)))) {
      // X se je odprla
      openEvent(x);
    }
  }
  prevStates = currentStates;
}
byte getStates() {
  byte retval = 0;
  for(byte x=0;x<1;x++) {
    int read = analogRead(A6);
    if(read < 100) {
      retval &= ~(1 << (2*x)); // 0
    }
    else {

      retval |= 1 << (2*x); // 1
    }
}
  return retval;
}

void serialCheck() {
  delay(20);
  byte data[MSG_LENGTH + 1];
  if (serial.available() == (MSG_LENGTH + 1)) {
    for (byte x = 0; x <= MSG_LENGTH; x++) {
      data[x] = serial.read();
    }
    for(byte x=0;x<= MSG_LENGTH;x++) {
      Serial.print(data[x], HEX); Serial.print(" ");
    }
    Serial.println();
    if (data[MSG_LENGTH] == '\n' and data[0] == ADDR) {
      byte crc8 = CRC8.smbus(data, sizeof(data) - 2);
      if (crc8 == data[sizeof(data) - 2]) {
        // Checksum OK
        // Preberi vsebino sporocila --------------------------------------------
        if (data[1] == Open and data[2] < 8 and data[2] >= 0) {
          // Odklep na daljavo
          unlock(data[2]);
          free(data);
          char b[1] = {OKByte};
          sendResponse(crc8, b, 1);

        }
        else if (data[1] == FLock and data[2] < 8 and data[2] >= 0) {
          // Prisilni zaklep
          lock(data[2]);
          free(data);
          char b[1] = {OKByte};
          sendResponse(crc8, b, 1);
        }
        else if (data[1] == Novo) {
          // Kaj je novega?
          if(data[2] == OKByte and temp_counter > 0) {
            // Izbriši prejšnji paket
            for (byte x = 0; x < BUFLEN; x++) {
              if (buf[x] != 0) {
                temp_counter--;
                buf[x] = 0;
                if (temp_counter == 0) break;
              }
            }
          }
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
        else if(data[1] == NfAd and (7 & data[2]) < 8) {
          // Shrani UID
          // Naslednji bajt je sestavljen iz št. omarice (zadnji 3 biti) in lokacije (prvi bit)
          // Sledi 7 bajtov UID-ja kartice - 4-bajtni UID ima na koncu ničle
          byte temp[7];
          for(byte x=0;x<7;x++) temp[x] = data[x+3];
          byte x = findByUID(temp);
          if(x == 255 or (x == (7 & data[2]))) {
            byte start = 14 * (7 & data[2])+7*((128 & data[2]) >> 7);
            for(byte x=0;x<7;x++) EEPROM.write(start + x, data[x + 3]);
            char b[1] = {OKByte};
            sendResponse(crc8,b,1);
          }
          else {
            // Kartica je že v bazi podatkov
            char b[1] = {NFC_DUPLICATE_ERR};
            sendResponse(crc8,b,1);
          }
        }
        else if(data[1] == NfRm and (7 & data[2]) < 8) {
          // Odstrani kartico
          byte start = 14 * (7 & data[2])+7*((128 & data[2]) >> 7);
          for(byte x=0;x<7;x++) EEPROM.write(start + x, 0);
          char b[1] = {OKByte};
          sendResponse(crc8,b,1);
        }
        // Preberi vsebino sporocila --------------------------------------------
      }
    }
  }
  else {
    while (serial.available()) serial.read();
  }
}

void sendResponse(const char * crc8, const char response[], const char len) {
  if (len > MSG_LENGTH - 3) {
    return; // za vsak slučaj
  }
  char data[MSG_LENGTH + 1];
  memset(data, 0, sizeof(data));
  data[0] = REPLY;
  data[1] = crc8;
  for (byte x = 0; x < len; x++)  {
    data[x + 2] = response[x];
  }
  data[MSG_LENGTH - 1] = CRC8.smbus(data, sizeof(data) - 2);
  data[MSG_LENGTH] = '\n';
  digitalWrite(TOGGLE, HIGH);
  for (byte x = 0; x <= MSG_LENGTH; x++)  {
    serial.write(data[x]);
  }
  serial.flush();
  digitalWrite(TOGGLE, LOW);
}

void unlock(byte num) {
  // Odkleni omarico 0-7 (servo.write)
  if(num > 7) return;
  if(num == 0) servo0.write(UNLOCK);
  Serial.print("unlock ");Serial.println(num);
  delay(150);
}

void lock(byte num) {
  // Zakleni omarico
  if(num > 7) return;
  if(num == 0) servo0.write(LOCK);
  Serial.print("lock ");Serial.println(num);
}

void openEvent(byte num) {
  // Funkcija se pokliče, ko se omarica odpre
  if (num > 7) return;
 // Serial.print("openEv ");Serial.println(num);
  if(1 & (waiting >> num)) {
    waiting &= ~(1 << num); //0
    return;
  }
  if (abs(millis() - times[num]) > 1000) {
    // Odklep ni bil napovedan
    // Zadnji 3 biti: št. omarice, naslednja 2 način odklepa
    Serial.println("nenap");
    addEvent(0b00001000 | num);
  }
}

void closeEvent(byte num) {
  if(num > 7) return;
  waiting |= 1 << num; //1
  times[num] = millis();
 // Serial.print("closeEv ");Serial.print(num);Serial.print(" "); Serial.println(times[num]);
 
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

byte findByUID(byte uid[]) {
  // Preišče lokalno bazo in vrne omarico, ki pripada tej kartici. Če ne obstaja, vrne 255.
  for(byte x=0;x<8;x++) {
    for(byte y=0;y<7;y++) {
      if(EEPROM.read(14*x+y) != uid[y]) break;
      if(y == 6) return x;
    }
    for(byte y=7;y<14;y++) {
      if(EEPROM.read(14*x+7+y) != uid[y]) break;
      if(y == 13) return x;
    }
  }
  return 255;
}
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID;
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
// serial.write(data[x]);
// }
// serial.flush();
// digitalWrite(TOGGLE, LOW);
// }
