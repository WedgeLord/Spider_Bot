#ifndef SPIDER_SERVER
#define SPIDER_SERVER

#include "spider_types.h"

typedef void setMotor_callback_sig( spider_leg_t );
typedef tie_return tie_callback_sig( tie_params );
// sets up a password-less access point and server to host a webpage control panel
void startSpiderServer( 
    setMotor_callback_sig *setMotor_callback, 
    tie_callback_sig *tie_callback 
);

#endif