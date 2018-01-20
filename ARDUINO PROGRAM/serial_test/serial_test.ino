// Program, ki vse, kar dobi, poslje na software serial. Vse dobljene odgovore izpisuje v hex obliki in v ascii-ju.

#include <SoftwareSerial.h>
#include <FastCRC.h>
FastCRC8 CRC8;

SoftwareSerial serial(6, 7);
void setup() {
  Serial.begin(9600);
  serial.begin(9600);
  Serial.println('0',HEX);
}

void loop() {
  if (Serial.available()) {
    delay(20);
    char data[9];
    data[0]='a';
    for (byte x = 0; x < 8; x++) {
      data[x+1] = Serial.read();
    }
    Serial.print("serial: ");
    for (byte x = 0; x < 8; x++) {
      Serial.print(data[x], HEX);
      Serial.print(' ');
    }
    Serial.println();
    
    for (byte x = 0; x < 9; x++) {
      serial.write(data[x]);
    }
    serial.write(CRC8.smbus(data, sizeof(data)));
    serial.write('\n');
    Serial.print("CRC8: "); Serial.println(CRC8.smbus(data, sizeof(data)),HEX);
  }
  if (serial.available()) {
    delay(20);

    String str = serial.readStringUntil('\n');
    Serial.print("got: ");
    Serial.print(str);
    Serial.print(" hex: ");
    for (byte x = 0; x < str.length(); x++) {
      Serial.print((byte)str.charAt(x),HEX);
      Serial.print(' ');
    }
    Serial.println();
    char b[9];
    for(byte x=0;x<9;x++) {
      b[x] = str.charAt(x);
    }
    Serial.print("rCRC: ");
    Serial.println(CRC8.smbus(b, sizeof(b)),HEX);
  }
}
