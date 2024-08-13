#include <Wire.h>
#define MASTER

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

void setup() {
  // put your setup code here, to run once:
  Serial.begin( 9600 );
  Wire.begin(0x5);
  Wire.setClock( 100000 );
  Wire.onReceive( handler );
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
  //while ( Wire.available() ) {
  for (int i = 0; i < bytes; i++) {
    Serial.print( (char) Wire.read() );
  }  
}

