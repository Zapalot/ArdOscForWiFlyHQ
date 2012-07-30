#include <WiFlyHQ.h>
#include <ArdOSCForWiFlyHQ.h>




#include <SoftwareSerial.h>
#define softSerialRx 2
#define softSerialTx 3


SoftwareSerial softSerial(softSerialRx,softSerialTx);
Stream* wiflySerial =&softSerial;

/* Change these to match your WiFi network */
const char mySSID[] = "SommerInAltTreptow";
const char myPassword[] = "0mme-5qut-ayrr";
WiFly wifly;

uint16_t localPort =8000;

uint16_t remotePort =8000;
const char remoteHost[]="192.168.1.17";



OSCServer server;


void setup()
{ 
	setupWiFly();
	debugWiFly();	//show some debug information


  {
   //start server
   server.begin();
   
   //set callback function
   server.addCallback("/ard/aaa",&func1);
   
   pinMode(ledPin, OUTPUT);  
  }

}

  
void loop()
{
  if(server.aviableCheck()>0)
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


void setupWiFly(){
  char buf[32];
  softSerial.begin(9600);

  if (!wifly.begin(wiflySerial, &Serial)) {
    Serial.println("Failed to start wifly");
  }

  wifly.setDeviceID("Wifly-UDP");
  Serial.print("DeviceID: ");
  Serial.println(wifly.getDeviceID(buf, sizeof(buf)));
  
  if (wifly.getFlushTimeout() != 10) {
    Serial.println("Restoring flush timeout to 10msecs");
    wifly.setFlushTimeout(10);
    wifly.save();
    wifly.reboot();
  }

  /* Join wifi network if not already associated */
  if (!wifly.isAssociated()) {
    /* Setup the WiFly to connect to a wifi network */
    Serial.println("Joining network");
    wifly.setSSID(mySSID);
    wifly.setPassphrase(myPassword);
    wifly.enableDHCP();

    if (wifly.join()) {
      Serial.println("Joined wifi network");
    } 
    else {
      Serial.println("Failed to join wifi network");
    }
  } 
  else {
    Serial.println("Already joined network");
  }
  /* Setup for UDP packets, sent automatically */
  wifly.setIpProtocol(WIFLY_PROTOCOL_UDP);
  wifly.setHost(remoteHost, remotePort);	// Send UPD packets to this server and port
  
  //receive packets at this port...
  if(wifly.getPort()!=localPort){
    Serial.println("Setting port to localPort and rebooting...");
    wifly.setPort( localPort);	// Send UPD packets to this server and port
    wifly.save();
    wifly.reboot();
  }
      wifly.save();
    Serial.println("WiFly ready");
}
void debugWiFly(){
  char buf[32];
  /* Ping the gateway */
  wifly.getGateway(buf, sizeof(buf));

  Serial.print("ping ");
  Serial.print(buf);
  Serial.print(" ... ");
  if (wifly.ping(buf)) {
    Serial.println("ok");
  } 
  else {
    Serial.println("failed");
  }

  Serial.print("ping google.com ... ");
  if (wifly.ping("google.com")) {
    Serial.println("ok");
  } 
  else {
    Serial.println("failed");
  }

  Serial.print("MAC: ");
  Serial.println(wifly.getMAC(buf, sizeof(buf)));
  Serial.print("IP: ");
  Serial.println(wifly.getIP(buf, sizeof(buf)));
  Serial.print("Netmask: ");
  Serial.println(wifly.getNetmask(buf, sizeof(buf)));
  Serial.print("Gateway: ");
  Serial.println(wifly.getGateway(buf, sizeof(buf)));


  Serial.print("Current Port:");
  Serial.println(wifly.getPort());

}