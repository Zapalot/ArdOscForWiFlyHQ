/*
 
 ArdOSC 2.1 - OSC Library for Arduino.
 --> Modifications made to the Version retrieved from https://github.com/watterott/RedFly-Shield
 -->  Adapted for the WiFlyHQ Library
 --> Plenty of modifications have been made by Watterott and me, so most errors you will encounter are probably not the original authors fault...
 -------- Lisence -----------------------------------------------------------
 
 ArdOSC
 
 The MIT License
 
 Copyright (c) 2009 - 2011 recotana( http://recotana.com )　All right reserved
 
 */

#include <stdlib.h>

#include "OSCCommon/OSCClient.h"


OSCClient::OSCClient(WiFly* wiFly):
wiFly(wiFly)
{

}


OSCClient::~OSCClient(void)
{

}


int OSCClient::send(OSCMessage *_message)
{
    int result = 1;
    uint16_t msgSize;
    uint8_t *sendData;

    msgSize  = _message->getMessageSize();
    sendData = (uint8_t*) calloc(msgSize, 1);

    if( sendData == NULL )
    {
      return -1;
    }

    if( encoder.encode(_message, sendData) < 0 )
    {
      free(sendData);
      return -2;
    }

    wiFly->write(sendData, msgSize);
    wiFly->flush();
	delay(2);	//wait for the wifly to time out...
    free(sendData);

    return result;
}
