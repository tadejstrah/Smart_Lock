
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>

int getFingerprintIDez();

String arduinoAddress = "01";

String receivedAddress;
String receivedCommand;
String receivedData;

bool stringComplete;
String inputString;
char inChar;

String num1;

bool looking_for_finger;
bool enrollingFinger;
bool deletingFingerprint;

int num;
uint8_t id;


SoftwareSerial mySerial(11, 10);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);




void setup(){

   Serial.begin(9600);

  finger.begin(57600);
  
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
   // while (1);
  }


}

void loop(){


   while(looking_for_finger){
            getFingerprintID();
            delay(50);
   }

   while(enrollingFinger){
         Serial.println("Ready to enroll a fingerprint!");
        Serial.println("Please type in the ID # (from 1 to 127) you want to save this finger as...");
        id = num;
        if (id == 0) {// ID #0 not allowed, try again!
           return;
        }
        Serial.print("Enrolling ID #");
        Serial.println(id);
        
        while (!  getFingerprintEnroll() );
   }

   while(deletingFingerprint){
        Serial.println("Please type in the ID # (from 1 to 127) you want to delete...");
        
        id = num;
        if (id == 0) {// ID #0 not allowed, try again!
           return;
        }

        Serial.print("Deleting ID #");
        Serial.println(id);
        
        deleteFingerprint(id);
   }

   if(stringComplete){


      receivedAddress = inputString.charAt(0);
      receivedAddress += inputString.charAt(1);

      receivedCommand = inputString.charAt(2);
      for (int i = 3; i < 6; ++i)
      {
         receivedCommand += inputString.charAt(i);
      }

      receivedData = inputString.charAt(6);
      for (int i = 7; i < 16; ++i)
      {
         receivedData += inputString.charAt(i);
      }


      if(receivedAddress == arduinoAddress){

          Serial.println("inut string je" + inputString);

          if (receivedCommand == "FpGt"){
            Serial.println("Dubu FpGt + "+ receivedData);
 
              looking_for_finger = true; 
              Serial.println("dej prst na senzor");
          }
         /* else if (receivedCommand == "FpRm"){
             Serial.println("dubu FpRm + "+ receivedData);
          }
          */
          else if (receivedCommand == "FpEn"){

               /*uint8_t readnumber(void) {
                 uint8_t num = 0;
                 
                 while (num == 0) {
                   while (! Serial.available());
                   num = Serial.parseInt();
                 }
                 return num;
               }

               */
               Serial.println("received data = " + receivedData);
               num1 = receivedData.charAt(0);
               for(int i=1; i<(receivedData.length()); i++){
                   num1 += receivedData.charAt(i);
               }

               num = num1.toInt();

              // num = (receivedData.charAt(9)-48)*10;
               //num = num + (receivedData.charAt(10)-48) ;
               Serial.print("num1=");
               Serial.println(num1);
               Serial.print("num=");
               Serial.println(num);

               enrollingFinger = true;

          }

          else if (receivedCommand == "FpRm"){
               num1 = receivedData.charAt(0);
               for(int i=1; i<(receivedData.length()); i++){
                   num1 += receivedData.charAt(i);
                    }

               num = num1.toInt();
               Serial.println("deleting finger z id-jem" + num);

              deletingFingerprint = true;
          }


          else if (receivedCommand == ""){          {
           Serial.println("dubu prazno");
          }
         /* else{
            Serial.println("dubu drugo komando" +  receivedCommand); 

          }
         */  
          receivedCommand = "";
       }
    inputString ="";
    stringComplete = false;
  
  }
  
}

}



void serialEvent()
{
  while (Serial.available())
  {                                    // get the new byte:
    char inChar = (char)Serial.read(); // add it to the inputString:
    inputString += inChar;             // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n')
    { // sign plus complets the string reading proces
      stringComplete = true;
    }
    if (inputString.length() == 0){
      Serial.println("dubu empty string na srialu");
    }   

  }
}



uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
     // Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  // OK converted!
  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match. Try again");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
  
  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  Serial.print(" with confidence of "); Serial.println(finger.confidence); 
  looking_for_finger = false;
  Serial.println("LOOKING For fp set to false");
  Serial.println("_________________________");
  
}







uint8_t getFingerprintEnroll() {

  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  Serial.println("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  // OK converted!
  Serial.print("Creating model for #");  Serial.println(id);
  
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
        enrollingFinger = false;
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
  
  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
    enrollingFinger = false;
    return; // ?
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
}



uint8_t deleteFingerprint(uint8_t id) {
  uint8_t p = -1;
  
  p = finger.deleteModel(id);

  if (p == FINGERPRINT_OK) {
    Serial.println("Deleted!");
    deletingFingerprint = false;
    return;
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not delete in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.print("Unknown error: 0x"); Serial.println(p, HEX);
    return p;
  }   
}