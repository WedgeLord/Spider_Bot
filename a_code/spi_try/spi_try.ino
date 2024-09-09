#include "spider_types.h"

volatile uint8_t val = 0;
volatile bool received = false;
volatile bool collision = false;
volatile uint8_t command = 0;

char msg[] = "sup";
int index = 0;
uint8_t input[MAX_TRANSMISSION] = {0};
uint8_t output[MAX_TRANSMISSION] = {0};
uint8_t *in_ptr = input;  // pointer to parameter struct
uint8_t *out_ptr = output; // pointer to bounds struct

ISR( SPI_STC_vect ) {
  digitalWrite( A1, HIGH );
  val = SPDR;

  if ( command == 0 ) {
    command = val;
    Serial.print( "Command: " );
    Serial.println( command );
    digitalWrite( A1, LOW );
    return;
  }

  received = true;
  digitalWrite( A1, LOW );
}

void setMotor() {

}

void tie() {
  if ( index < sizeof( tie_params ) ) { 
    *in_ptr = val;
    in_ptr++;
  }
  
  // prepare next byte to transmit
  index += 1;
  if ( index >= sizeof( tie_params ) + sizeof( tie_return ) ) {
    resetSPI();
  }    
  else if ( index >= sizeof( tie_params ) ) {
    SPDR = *out_ptr;
    out_ptr++;
  }
}

void resetSPI() {
  command = 0;
  index = 0;
  in_ptr = input;
  out_ptr = output;
  Serial.println("spi reset");
}

void setup() {
  SPCR = _BV(SPIE) | _BV(SPE);
  SPDR = '?';
  *((tie_return *) output) = { { 8, 12 }, { 64, 128 } };
  Serial.begin( 9600 );
  pinMode( MISO, OUTPUT );
  pinMode( A1, OUTPUT );
  digitalWrite( A1, LOW );
  Serial.println( "SETUP COMPLETE" );
}

void loop() {
  if ( received ) {
    received = false;

    switch ( command ) {
      case 1:
        setMotor();
        break;
      
      case 2:
        tie();
        break;
      
      default:
        Serial.println( "Command not recognized" );
    }
  }
  if ( SPSR & _BV(WCOL) ) { // unset WCOL by reading SPSR, then SPDR
    Serial.println( "COLLISION" );
    collision = false;
  }
}
