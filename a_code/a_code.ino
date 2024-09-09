#include <Wire.h>
#include <Servo.h>
#include "spider/spider.h"

#ifdef HI

void setup() {
  Serial.begin( 9600 );
  Wire.begin( 0x55 );
  Wire.onReceive( read );
  Wire.onRequest( reply );
  Serial.println( "setup finished" );
}

void loop() {
  // put your main code here, to run repeatedly:
  // Serial.println( "\n[P] looping..." );
  // delay( 2000 );
}

void reply() {
  // uint8_t msg = 123;  
  static uint8_t msg[2] = { 22, 44 };
  Wire.write( msg, sizeof( msg ) );
  for ( int t = 0; t < 4; t++ ) {
    Serial.println( "1234567890" );
  }    
  // Serial.println("sending");
}
void read( int bytes ) {
  char buffer[bytes];
  // Serial.print( "Command: " );  Serial.println( Wire.read() );

  for ( int i = 0; i < bytes; i++ ) {
    buffer[i] = Wire.read();
  }
  Serial.println( buffer );
}

#endif
