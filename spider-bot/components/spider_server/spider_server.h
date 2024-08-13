#ifndef SPIDER_SERVER
#define SPIDER_SERVER

// datatype to hold motor information
typedef struct {
    int leg;    // between 1-4 (it's a quadroped robot)
    int height; // the angle of the height-motor
    int pivot;  // the angle of the pivot-motor
} spider_leg_t;

// sets up a password-less access point and server to host a webpage control panel
void startSpiderServer( spider_leg_t *user_spider_leg );

#endif