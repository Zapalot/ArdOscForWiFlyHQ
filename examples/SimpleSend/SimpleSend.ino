// How to send OSC messages from an Arduino.
// This Example is in the public domain.
#include <WiFlyHQ.h>
#include <ArdOSCForWiFlyHQ.h>

//uncomment this to use a software serial if you dont own a MEGA
//#include <SoftwareSerial.h>
//SoftwareSerial softSerial(softSerialRx,softSerialTx);

WiFly wifly;
OSCClient client(&wifly);
//create new osc message
OSCMessage global_mes;

int globalIntValue=0;
float globalFloatValue=1000;
char* globalString="foo";
void setup()
{ 
  Serial.begin(115200);
  //use the convenient setup. Replace <HardwareSerial> by <SoftwareSerial> if you use one.
  wifly.setupForUDP<HardwareSerial>(
    &Serial3,   //the serial you want to use (this can also be a software serial)
    115200, // if you use a hardware serial, I would recommend the full 115200
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

}


void loop()
{
  //note that the destination adress is set by setting the remote host of the wifly!  
  //three ways of sending messages...
  
  //simple but not as flexible: the convenience functions:
  client.sendInt(analogRead(A0),"/ard/A0Value");
  client.sendFloat((float)analogRead(A1)*5.0/255.0,"/ard/A1Voltage");

  //using a local message object allows to multiple additional parameters in a singel message:
  //loacal_mes,str is release by out of scope
  OSCMessage loacal_mes;
  loacal_mes.beginMessage("/ard/A2A3Value");
  loacal_mes.addArgInt32(analogRead(A2));
  loacal_mes.addArgInt32(analogRead(A3));
  client.send(&loacal_mes);
  
  //using a global message object and some global variables works as well
  global_mes.beginMessage("/ard/status");
  global_mes.addArgInt32(globalIntValue);
  global_mes.addArgFloat(globalFloatValue);
  global_mes.addArgString(globalString);

  client.send(&global_mes);
  global_mes.flush(); //object data clear

  
  globalIntValue++;
  globalFloatValue*=0.999;
  

}

