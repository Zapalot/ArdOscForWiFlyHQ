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

unsigned long receiveTimeout= 2; // adjust to match your Baudrate: Should be around 2-3 with 115200 Baud, higher at lower Baudrates
WiFly wifly;
OSCServer server(&wifly);

int ledPin=13;

void setup()
{ 
  Serial.begin(115200);
  //use the convenient setup. Replace <HardwareSerial> by <SoftwareSerial> if you use one.
  wifly.setupForUDP<HardwareSerial>(
  &Serial3,   //the serial you want to use (this can also be a software serial)
  115200,      // if you use a hardware serial, I would recommend the full 115200. 28800 is the maximum that works reliably with a software serial.
  true,	// should we try some other baudrates if the currently selected one fails?
  "wally",    //Your Wifi Name (SSID)
  "robomint", //Your Wifi Password 
  "WiFly",    // Device name for identification in the network
  0,          // IP Adress of the Wifly. if 0 (without quotes), it will use dhcp to get an ip
  8000,                   // WiFly receive port
  "255.255.255.255",       // Where to send outgoing Osc messages. "255.255.255.255" will send to all hosts in the subnet
  8001,                     // outgoing port
  true	// show debug information on Serial
  );
  wifly.printStatusInfo(); //print some debug information 

  pinMode(ledPin,OUTPUT); //prepare to fade th einternal LED by OSC
  // Register a callback fuction of the LED fading.
  // When a message is with the adress string "/ard/ledFade" is received, the function "setLedFade" will be invoked.
  server.addCallback("/ard/ledFade",&setLedFade);
}


void loop()
{
  //check for new incoming messages. If a message with a registered adress is recieved, the callback function (in this case setLedFade()) will be invoked.
  if(server.availableCheck(receiveTimeout)>0)
  {  
    //debugoutln("alive!"); //callback after process
  }
}

void setLedFade(OSCMessage *_mes){
  //get first argument of the osc message (0), try to interpret it as an integer (Int32) and use it to fade the LED. Other Argument types are getArgFloat or getArgString
  int fadeValue=_mes->getArgInt32(0); // the '->' allows access to functions of the message that was passsed as a pointer (*). 
  Serial.println(fadeValue);
  analogWrite(ledPin,fadeValue );     // set the led brightness to the received value
}


