#include <SoftwareSerial.h>
SoftwareSerial s(6, 7);
/*
  RS485 - adresiran slave; odgovori na vse kar SE začne na 'a'
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
  Serial.begin(9600);

  pinMode(2, OUTPUT);

  digitalWrite(2, LOW);
  s.begin(19200);
}


void loop()
{

  if (s.available()) {
    delay(10);
    String aa = "";
    while (s.available()) {
      char c = s.read();
      aa += c;
      delay(1);
    }
    delay(10);
    if (aa.startsWith("a")) {
      digitalWrite(2, HIGH);

      s.print("aarecv: ");
      s.print(aa);
      s.print('\n');

      s.flush();
      Serial.println(aa);
      Serial.println(aa);
      digitalWrite(2, LOW);
    }
  }
}
