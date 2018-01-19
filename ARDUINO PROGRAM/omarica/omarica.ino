#define ADDR 'a'
#define MSG_LENGTH 10
#include <FastCRC.h>

FastCRC8 CRC8;



void setup() {
  Serial.begin(9600);

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
        for (byte x = 0; x <= MSG_LENGTH; x++) {
          Serial.print(data[x]);
        }
        Serial.println();
        Serial.println( CRC8.smbus(data, sizeof(data)), HEX);
      }
    }
    else {
      while (Serial.available()) Serial.read();
    }
  }
}

void serialEvent() {

}

