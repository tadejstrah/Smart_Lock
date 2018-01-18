#include <Arduino.h>
#ifndef protocol
#define protocol




class Protocol
{
public:
	Protocol();
	void SerialReceived();
	void serialEvent1();
	
	String getAddress();
	String getData();
	String getCommand();
	String receivedAddress;
	String inputString;
	String receivedCommand;
	String receivedData;
	bool stringComplete;
	char inChar;
	
};




#endif