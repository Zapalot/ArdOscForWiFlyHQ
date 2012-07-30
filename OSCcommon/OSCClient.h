/*
 
 ArdOSC 2.1 - OSC Library for Arduino.
 
 -------- Lisence -----------------------------------------------------------
 
 ArdOSC
 
 The MIT License
 
 Copyright (c) 2009 - 2011 recotana( http://recotana.com )　All right reserved
 
 */

#ifndef ArdOSC_OSCClient2_h
#define ArdOSC_OSCClient2_h


#include "OSCcommon.h"
#include "OSCMessage.h"
#include "OSCEncoder.h"

#include "../WiFlyHQ/WiFlyHQ.h"

#define kDummyPortNumber 10000


class OSCClient{
    
private:

    WiFly* wiFly;

    OSCEncoder::OSCEncoder encoder;

public:

    OSCClient(WiFly* wiFly);
    ~OSCClient(void);
    int send(OSCMessage *_message);

};


#endif
