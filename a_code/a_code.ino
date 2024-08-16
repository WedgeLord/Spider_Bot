#include <Wire.h>
#include <Servo.h>
#include "spider/spider.h"
// #define MASTER

void handler(int bytes);
#ifdef MASTER

void setup() {
  // put your setup code here, to run once:
  Serial.begin( 9600 );
  Wire.begin();
  Wire.setClock( 10000 );
  //Wire.onReceive( handler );
  /*
  for (unsigned int i = 8; i < 127; i++) {
    Wire.requestFrom( i, 1 );
    if ( Wire.available() ) {
      Wire.read();
      Serial.print( "found device at ");
      Serial.println( i );
    }
  }*/
}

void loop() {
  // put your main code here, to run repeatedly:
  Wire.requestFrom( 0x33, 5);
  Wire.beginTransmission( 0x33 );
  if ( Wire.available() ) {
    Wire.write( "received stuff" );
  }
  else {
    Wire.write( "not connected?" );
  }
  Wire.endTransmission();
  while ( Wire.available() ) {
    Serial.print( (char) Wire.read() );
  }

  Serial.println( "\n[C] looping..." );
  delay( 1000 );
}

#else
void reply();
Servo foot, hip;
void setup() {
  // turn off all motors
  // Spider::getBot().destroy();
  Serial.begin( 9600 );
  Wire.begin( 0x55 );
  // hip.attach( 3, 700, 2300 ); // set to eliminate stalling
  // foot.attach( 6, 900, 910 );
  // Wire.onReceive( handler );
  Wire.onRequest( reply );
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println( "\n[P] looping..." );
  delay( 1000 );
}

void reply() {
  Wire.write( "wassup!" );
}

#endif

void handler(int bytes) {
  spider_leg_t leg;
  Wire.readBytes( (char *)&leg, bytes );
  Serial.print( leg.leg );
  Serial.print( leg.height );
  Serial.println( leg.pivot );
  foot.write( leg.height );
  hip.write( leg.pivot );
}

