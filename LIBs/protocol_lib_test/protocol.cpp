#include <Arduino.h>
#include "protocol.h"

	String receivedAddress;
	String inputString;
	String receivedCommand;
	String receivedData;
	bool stringComplete;
	char inChar;

Protocol::Protocol( ){

}

void Protocol::SerialReceived(){

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
    	inputString ="";
    	stringComplete = false;
  
   

}

String Protocol::getAddress(){
  receivedAddress = inputString.charAt(0);
  receivedAddress += inputString.charAt(1);
	return receivedAddress;//receivedAddress;
}


String Protocol::getData(){
  receivedData = inputString.charAt(6);
      for (int i = 7; i < 16; ++i)
      {
         receivedData += inputString.charAt(i);
      }
	return receivedData;

}



String Protocol::getCommand(){
  receivedCommand = inputString.charAt(2);
      for (int i = 3; i < 6; ++i)
      {
         receivedCommand += inputString.charAt(i);
      }
	return receivedCommand;

}







 




void Protocol::serialEvent1()
{
  while (Serial.available())
  {                                    // get the new byte:
    inChar = (char)Serial.read(); // add it to the inputString:
    inputString += inChar;             // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n')
    { // sign plus complets the string reading proces
      stringComplete = true;
    }
    if (inputString.length() == 0){
      Serial.println("dubu empty string na serialu");
    }   

  }
  
}
