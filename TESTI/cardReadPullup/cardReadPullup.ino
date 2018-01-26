  #include <Wire.h>
  #include <PN532_I2C.h>
  #include <PN532.h>
  #include <NfcAdapter.h>
  
  PN532_I2C pn532i2c(Wire);
  PN532 nfc(pn532i2c);
  
const int buttonPin = 2;
const int transPin = 3;
int buttonState = 0; 
bool buttonState1 = 0;
bool tranState = false;

void setup() {
  Serial.begin(115200);
  pinMode(transPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  digitalWrite(transPin, HIGH);
}

void loop() {
  buttonState = digitalRead(buttonPin);
  //Serial.println(buttonState);
  if ((buttonState == LOW) and (buttonState1 == HIGH)) {
    tranState =!(tranState);
    Serial.println("transistor:" + String(tranState));
    digitalWrite(transPin, !tranState);
    
    if (tranState){
      Serial.println("Hello");
      
      nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1);
  }
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
  nfc.setPassiveActivationRetries(0x01);
    nfc.SAMConfig();
    Serial.println("Waiting for an ISO14443A card");
    }
    }
  if(tranState){
      boolean success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0};
//  uint8_t CheckUid[4] = {144, 21, 25, 75};
  uint8_t uidLength;
  byte countLength;                        

  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength);
  
  if (success) {
    Serial.println("Found a card!");
    Serial.print("UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    Serial.print("UID Value: ");
    for (uint8_t i=0; i < uidLength; i++) 
    {
      Serial.print(" 0x");Serial.print(uid[i], HEX); 
      /*if (uidLength == 4){
        if (uid[i] == CheckUid[i]){
          countLength++; 
        }
      }
    }
    if (countLength == 4){
      digitalWrite(3, HIGH); 
    }
    else if(countLength != 4){
      digitalWrite(8, HIGH);
    }*/
    }
    Serial.println("");
    tranState = false;
    digitalWrite(transPin, !tranState);
    delay(1000);
    //digitalWrite(3, LOW); 
    //digitalWrite(8, LOW); 

  }
  else
  {
    delay(1);
    Serial.println("Timed out waiting for a card");
  }
  }
    
  buttonState1 = buttonState;
  delay(1);
  }
  
