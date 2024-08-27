#include "spider.h"
#include <Wire.h>

void onSetMotor();

Spider *bot;

void setup() {
  Serial.begin( 9600 );
  bot = Spider::getBot();
  Wire.begin( 0x55 );
  Wire.onReceive( onSetMotor );
  Serial.println( "setup() complete" );
  // bot->walkSquare( "s" );
  // bot->walkSquare( "f" );
  bot->walkSquare( "t" );
}

void loop() {

}

void onSetMotor( int bytes ) {
  Serial.print( bytes );
  Serial.println( " bytes received\n");
  spider_leg_t spider_leg;
  int read_bytes = Wire.readBytes( (char *)&spider_leg, bytes );
  if ( bytes != read_bytes ) {
    Serial.println( "read failed" );
  }
  Serial.println( spider_leg.leg );
  Serial.println( spider_leg.height );
  Serial.println( spider_leg.pivot );
  bot->setMotor( spider_leg );
}
