// How to send OSC messages from an Arduino.
// This Example is in the public domain.

// Important known issues:
// The Wifly provides no means to get the data from a single UDP package at a time
// Everything is just dumped to the serial connection.
// The only hint that a package is finished is that reading from the serial *might* time out
// To make things work anyway:
// 1. Use a high baud rate for communication with the Wifly: 115200 on a hardware serial is perfect.
// 2. Adjust the timeout in server.availableCheck(). 2ms works fine with 115200baud.
// 3. If possible, let the controlling Program pause at least 1.5 times the timout between sending OSC messages.

#include <WiFlyHQ.h>
#include <ArdOSCForWiFlyHQ.h>

//uncomment this to use a software serial if you dont own a MEGA
//#include <SoftwareSerial.h>
//SoftwareSerial softSerial(softSerialRx,softSerialTx);

WiFly wifly;
OSCServer server(&wifly);

int ledPin=13;

void setup()
{ 
  Serial.begin(115200);
  //use the convenient setup. Replace <HardwareSerial> by <SoftwareSerial> if you use one.
  wifly.setupForUDP<HardwareSerial>(
    &Serial3,   //the serial you want to use (this can also be a software serial)
    115200, // if you use a hardware serial, I would recommend the full 115200. This does not work with a software serial.
    true,	// should we try some other baudrates if the currently selected one fails?
    "WLAN-466B23",  //Your Wifi Name (SSID)
    "SP-213B33501", //Your Wifi Password 
    "WiFly",                 // Device name for identification in the network
    "192.168.2.201",         // IP Adress of the Wifly. if 0 (without quotes), it will use dhcp to get an ip
    8000,                    // WiFly receive port
    "255.255.255.255",       // Where to send outgoing Osc messages. "255.255.255.255" will send to all hosts in the subnet
    8001,                     // outgoing port
    true	// show debug information on Serial
  );
  wifly.printStatusInfo(); //print some debug information 

  pinMode(ledPin,OUTPUT); //prepare to fade th einternal LED by OSC
   // Register a callback fuction of the LED fading.
   // When a message is with the adress "/ard/ledFade" is received, "setLedFade" will be invoked.
   server.addCallback("/ard/ledFade",&setLedFade);
}


void loop()
{
	//check for new incoming messages. If a message with a registered adress is recieved, the callback function will be invoked.
  if(server.availableCheck(2)>0)
  {  
    //debugoutln("alive!"); //callback after process
  }
}

void setLedFade(OSCMessage *_mes){
  //get 1st argument(int32) and use it to fade the LED
  int fadeValue=_mes->getArgInt32(0);
  Serial.println(fadeValue);
  analogWrite(ledPin,fadeValue );
}

