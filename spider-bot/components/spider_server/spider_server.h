#ifndef SPIDER_SERVER
#define SPIDER_SERVER

#include "spider_types.h" // probably belongs in .c file but the spider_leg_t below would be undef
// datatype to hold motor information
// typedef struct {
//     int leg;    // between 1-4 (it's a quadroped robot)
//     int height; // the angle of the height-motor
//     int pivot;  // the angle of the pivot-motor
// } spider_leg_t;

typedef void tie_callback_sig( tie_params );
// sets up a password-less access point and server to host a webpage control panel
void startSpiderServer( spider_leg_t *user_spider_leg, tie_callback_sig *user_callback );

#endif