#include "./spider.h"

volatile char val = 0;
volatile bool received = false;
volatile bool collision = false;
char msg[] = "sup";
volatile int index = 0;
volatile tie_return bounds = { { 0, 45 }, { 30, 90 } };
volatile uint8_t *b_ptr = (uint8_t *) &bounds;
volatile tie_params params = {0};
volatile uint8_t *p_ptr = (uint8_t *) &params;

ISR( SPI_STC_vect ) {
  digitalWrite( A1, HIGH );
  if ( index == 0 ) {
    Serial.print( "Command: " );
    Serial.println( SPDR );
    index++;
    digitalWrite( A1, LOW );
    return;
  }
  // SPDR = 0xD4;
  // for ( int i = 0; i < 1; i++ ) Serial.print( "1234567890" );
  // Serial.println();  
  if ( index < sizeof( tie_params ) + 1 ) { 
    *p_ptr = SPDR;
    p_ptr++;
  }
  index += 1;
  if ( index > sizeof( tie_params ) + 1 ) {
    SPDR = *b_ptr;
    b_ptr++;
  }
  if ( index > sizeof( tie_params ) + sizeof( tie_return ) + 1 ) {
    index = 0;
    received = true;
  }
  collision = ( SPSR & _BV(WCOL) ); // unset WCOL by reading SPSR, then SPDR
  digitalWrite( A1, LOW );
}

void resetSPI() {
  index = 0;
  Serial.println("spi reset");
}

void setup() {
  SPCR = _BV(SPIE) | _BV(SPE);
  SPDR = '?';
  Serial.begin( 9600 );
  pinMode( MISO, OUTPUT );
  attachInterrupt( digitalPinToInterrupt( SS ), resetSPI, RISING );
  pinMode( A1, OUTPUT );
  digitalWrite( A1, LOW );
  Serial.println( "SETUP COMPLETE" );
}

void loop() {
  if ( received ) {
    Serial.print( (char*)&params );
    received = false;
  }
  if ( collision ) {
    Serial.println( "COLLISION" );
    collision = false;
  }
  // delay(1000);
}
