#include <WiFlyHQ.h>
#include <ArdOSCForWiFlyHQ.h>
#define DEBUG 1
#ifndef TRACE
#define TRACE(x) do { if (DEBUG) Serial.print( x); } while (0)
#endif
#ifndef TRACELN
#define TRACELN(x) do { if (DEBUG) Serial.println( x); } while (0)
#endif


#include <SoftwareSerial.h>
#define softSerialRx 2
#define softSerialTx 3

int ledPin=13;
SoftwareSerial softSerial(softSerialRx,softSerialTx);

WiFly wifly;
OSCServer server(&wifly);
int t=0;
void setup()
{ 
  configureWiFly(
  &softSerial, //the serial the wifly is attached to. assumes a softare serial. can be changed to hardware by editing the function below
  9600,        //baudRate   
     "Your WiFi Name",
    "Your WiFi Password",
    "WiFly",      // for identification in the network
    "192.168.2.201",         // IP Adress of the Wifly. if 0 (without quotes), it will use dhcp to get an ip
    8000,                    // WiFly receive port
    "255.255.255.255",       // Where to send outgoing Osc messages. "255.255.255.255" will send to all hosts in the subnet
    8001                     // outgoing port
    );

  printWiFlyInfo(); //print some debug information 

   //set callback function
   server.addCallback("/ard/aaa",&func1);
   
   pinMode(ledPin, OUTPUT);  

}

  
void loop()
{
  if(server.availableCheck()>0)
  {
    
    //debugoutln("alive!"); //callback after process
  }

  digitalWrite(ledPin, HIGH);
  delay(50);
  digitalWrite(ledPin, LOW);
  delay(t);
}


void func1(OSCMessage *_mes)
{
  //get 1st argument(int32)
  t = _mes->getArgInt32(0);
}


void configureWiFly(
SoftwareSerial* wiflySerial,
const uint32_t newSerialSpeed,
const char* SSID,
const char* password,
const char* deviceID,      ///< for identifacation in the network
const char* localIP,       ///< a string with numbers, if 0, we will use dhcp to get an ip
const uint16_t localPort,
const char* remoteHost,
const uint16_t remotePort
){
  TRACE((F("Free memory: ")));
  TRACELN((wifly.getFreeMemory()));

  boolean saveAndReboot=false;
  char buf[32];

  //try out some different serial speeds until we find one that is working...
  const uint32_t serialSpeeds[]={
    newSerialSpeed,9600, 19200, 38400, 57600, 115200      };

  int speedIndex=0;
  boolean baudrateFound=false;
  TRACELN(F("Trying to connect to WiFly"));
  for (speedIndex=0;speedIndex<5;speedIndex++){
    TRACE(F("Connecting to Wifly using Baudrate:"));
    TRACELN(serialSpeeds[speedIndex]);
    wiflySerial->begin(serialSpeeds[speedIndex]);
    if(wifly.begin(wiflySerial, &Serial)){
      baudrateFound=true;
      break;
    }
  }

  if(!baudrateFound){
    TRACELN(F("Could not find working Baud rate to connect to WiFly"));
    return;
  }
  else{
    TRACE(F("Working Baud rate is "));
    TRACELN(serialSpeeds[speedIndex]);
  }

  wifly.startCommand();  //made this public to avoid the annoing waiting times
  if(wifly.getBaud()!=newSerialSpeed){
    TRACE(F("Setting new baud rate to "));
    TRACELN(newSerialSpeed);
    //set the new wifly baud rate
    wifly.setBaud(newSerialSpeed);
    wiflySerial->begin(newSerialSpeed);
    //saveAndReboot=true;
  }

  wifly.getDeviceID(buf, sizeof(buf));
  if(strcmp(buf, deviceID) != 0){
    TRACE(F("Changing device ID to "));
    TRACELN(deviceID);
    wifly.setDeviceID(deviceID);
    saveAndReboot=true;
  }

  //setup dhcp or an ip..
  if(localIP[0]==0){
    if(wifly.getDHCPMode()!=WIFLY_DHCP_MODE_ON){
      TRACELN(F("Enabling DHCP"));

      wifly.enableDHCP();
      saveAndReboot=true;
    }
  }
  else{
    if(wifly.getDHCPMode()!=WIFLY_DHCP_MODE_OFF){
      TRACE(F("Disabling DHCP; setting IP to "));
      TRACELN(localIP);
      wifly.disableDHCP();
      wifly.setIP(localIP);
      saveAndReboot=true;
    }
    //set ip only if necessary...
    wifly.getIP(buf,sizeof(buf));  
    if(strcmp(buf, localIP) != 0){
      TRACE(F("Setting IP to "));
      TRACELN(localIP);
      wifly.setIP(localIP);
      saveAndReboot=true;
    }
  }

  //receive packets at this port...
  if(wifly.getPort()!=localPort){
    TRACE(F("Setting listen Port to "));
    TRACELN(localPort);
    wifly.setPort( localPort);	// Send UPD packets to this server and port
    saveAndReboot=true;
  }

  //2 millis seems to be the minimum for 9600 baud
  if (wifly.getFlushTimeout() != 2) {
    TRACE(F("Setting Flush timeout to 2ms "));
    wifly.setFlushTimeout(2);
    saveAndReboot=true;
  }
  
  //set SSID only if necessary...
  wifly.getSSID(buf,sizeof(buf));  
  if(strcmp(buf, SSID) != 0){
    TRACE(F("Setting SSID to "));
    TRACELN((SSID));
    wifly.setSSID(SSID);
    
    TRACE(F("Setting Wifi Passwd to "));
    TRACELN((password));
    wifly.setPassphrase(password);
    saveAndReboot=true;
  }
  
  wifly.save();
  wifly.finishCommand();
  if(saveAndReboot==true){
    wifly.reboot();
    wiflySerial->begin(newSerialSpeed);
  }


  
  //set SSID only if necessary...
  wifly.getSSID(buf,sizeof(buf));  
  if(strcmp(buf, SSID) != 0){
    wifly.setSSID(SSID);
    wifly.setPassphrase(password);
  }
      
      
  /* Join wifi network if not already associated */
  if (!wifly.isAssociated()) {
    /* Setup the WiFly to connect to a wifi network */
    TRACELN(F("Joining network"));
    wifly.setSSID(SSID);
    wifly.setPassphrase(password);

    if (wifly.join()) {
      TRACELN(F("Joined wifi network"));
    } 
    else {
      TRACELN(F("Failed to join wifi network"));
    }
  } 
  else {
    TRACELN(F("Already joined network"));
  }

  //enable auto join
  wifly.setJoin(WIFLY_WLAN_JOIN_AUTO);
  /* Setup for UDP packets, sent automatically */
  wifly.setIpProtocol(WIFLY_PROTOCOL_UDP);
  wifly.setHost(remoteHost, remotePort);	// Send UPD packets to this server and port


    TRACELN(F("WiFly setup finished"));
}
void printWiFlyInfo(){
  wifly.startCommand();
  char buf[32];
  /* Ping the gateway */
  wifly.getGateway(buf, sizeof(buf));

  TRACE("ping ");
  TRACE(buf);
  TRACE(" ... ");
  if (wifly.ping(buf)) {
    TRACELN("ok");
  } 
  else {
    TRACELN("failed");
  }

  TRACE("ping google.com ... ");
  if (wifly.ping("google.com")) {
    TRACELN("ok");
  } 
  else {
    TRACELN("failed");
  }

  TRACE("MAC: ");
  TRACELN(wifly.getMAC(buf, sizeof(buf)));
  TRACE("IP: ");
  TRACELN(wifly.getIP(buf, sizeof(buf)));
  TRACE("Netmask: ");
  TRACELN(wifly.getNetmask(buf, sizeof(buf)));
  TRACE("Gateway: ");
  TRACELN(wifly.getGateway(buf, sizeof(buf)));
  TRACE("Listen Port:");
  TRACELN(wifly.getPort());
  wifly.finishCommand();

}

