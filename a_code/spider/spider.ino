#include "spider.h"
#include <Wire.h>

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println( "\n[A] looping..." );
  delay( 2000 );
}

Spider *bot;
tie_return tie_response = {0};

void setup() {
  Serial.begin( 9600 );
  bot = Spider::getBot();
  Wire.begin( 0x55 );
  Wire.onReceive( onESPCommand );
  Wire.onRequest( onESPRequest );
  Serial.println( "setup() complete" );
}

void onESPRequest() {
  // apparently, can't waste any time with this function since it's in ISR and could get cut short automatically
  // so, no Serial.print's or delays
  // Wire.write( (uint8_t *) &tie_response, sizeof( tie_return ) );
  uint8_t buf[2] = { 80, 22 };
  Serial.println( "sending" );
  Wire.write( buf, sizeof(buf) );
  // tie_response = {0};
}

void onESPCommand( int bytes ) {
  tie_params params; // used by the tie() method
  tie_return leg_bounds;
  spider_leg_t spider_leg;  // used by pivot() method
  int read_bytes;

  Serial.println( Wire.available() );
  for ( int j = 0; j < 10; j++ ) 
    Serial.println("123456789012345678901234567890123456789012345678901234567890");
  while ( Wire.available() - 1 ) Wire.read();
  Serial.println( Wire.available() );
  Wire.read();

  return;
  // Serial.print( bytes );
  // Serial.println( " bytes received");
  uint8_t command = Wire.read();
  // Serial.print( "Command: " );
  Serial.println( command );
  switch ( command ) {
    case 1:
      Serial.println( "pivot command begin" );
      read_bytes = Wire.readBytes( (char *)&spider_leg, bytes );
      if ( bytes != read_bytes ) {
        Serial.println( "read failed" );
      }
      Serial.println( spider_leg.leg );
      Serial.println( spider_leg.height );
      Serial.println( spider_leg.pivot );
      bot->setMotor( spider_leg );
      break;

    case 2:
      if ( Wire.available() != sizeof( tie_params ) ) {
        Serial.println( "command failed" );
        return;
      }
      // Serial.println( "bind command begin" );
      Wire.readBytes( (char*) &params, bytes );
      // tie_response = bot->tie( params );

      // a successful upper bound would always be greater than 0
      // if ( tie_response.leg1.upper > 0 )
      //   Serial.println( "Bind Success!" );
      // else 
      //   Serial.println( "Bind Fail!" );
      break;

    default:
      Serial.println( "Command not recognized" );
      break;
  }
}
