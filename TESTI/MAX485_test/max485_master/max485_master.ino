#include <SoftwareSerial.h>
SoftwareSerial s(6, 7);
/*
  RS485 - master; v omrezje poslje vse, kar dobi po serialu, in vrne odgovor
  ---------------------
  Arduino --> MAX485
  pin 6   --> R0 (receive Out)
  pin 7   --> DI (digital In)
  pin 2   --> DE, RE
  5V, GND --> VCC, GND
  ---------------------
*/
void setup()
{
  Serial.begin(115200);

  pinMode(2, OUTPUT);

  digitalWrite(2, LOW);
  s.begin(19200);
}


void loop()
{

  if (Serial.available()) {
    String str = Serial.readStringUntil('\n');
    digitalWrite(2, HIGH); //tx
    s.print(str); s.print('\n');
    s.flush();
    digitalWrite(2, LOW); //rx
  }
  if (s.available()) {
    delay(10);
    String aa = "";
    while (s.available()) {
      char c = s.read();
      aa += c;
      delay(1);
    }
    Serial.print("slave: ");
    Serial.println(aa);
  }
}
