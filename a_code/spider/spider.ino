#include "spider.h"

// handler variables
int index = 0;
uint8_t input[MAX_SPI_TRANSMIT] = {0};
uint8_t output[MAX_SPI_TRANSMIT] = {0};
uint8_t *in_ptr = input;  // pointer to parameter struct
Spider *bot;

// ISR variables
volatile uint8_t val = 0;
volatile uint8_t command = 0;
volatile bool received = false;
volatile uint8_t *out_ptr = output; // pointer to bounds struct

ISR( SPI_STC_vect ) {
  digitalWrite( A1, HIGH );
  val = SPDR;

  if ( command == 0 ) {
    command = val;
    // Serial.print( "Command: " );
    // Serial.println( command );        
    digitalWrite( A1, LOW );
    // return;
  }
  else {
    index += 1; // process next byte    
    received = true;
  }
}

void setMotor() {
  if ( index <= sizeof( spider_leg_t ) ) {
    *in_ptr = val;
    in_ptr++;
  }

  if ( index >= sizeof( spider_leg_t) ) {
    bot->setMotor( *( spider_leg_t *)input );
    resetSPI();
  }
  digitalWrite( A1, LOW );
}

void tie() {
  if ( index <= sizeof( tie_params ) ) { 
    *in_ptr = val;
    in_ptr++;
  }
  
  // prepare bytes to transmit 
  if ( index == sizeof( tie_params ) ) {
    *(tie_return *)output = bot->tie( *(tie_params *)input );
  }

  if ( index >= sizeof( tie_params ) + sizeof( tie_return ) ) { // if transmission finished
    resetSPI();  
  }   

  // load in byte to transmit
  if ( index >= sizeof( tie_params ) ) {
    SPDR = *out_ptr;    
    out_ptr++;
  }

  digitalWrite( A1, LOW );
}

void resetSPI() {
  command = 0;
  index = 0;
  in_ptr = input;
  out_ptr = output;
  Serial.println("spi reset");
  digitalWrite( A1, LOW );
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
        command = 0;
        break;
    }
  }
  if ( SPSR & _BV(WCOL) ) { // unset WCOL by reading SPSR, then SPDR
    Serial.println( "COLLISION" );
  }
}

void setup() {
  Serial.begin( 9600 );
  bot = Spider::getBot();
  SPCR = _BV(SPIE) | _BV(SPE);
  *((tie_return *) output) = { { 8, 12 }, { 64, 128 } };  // test data
  pinMode( MISO, OUTPUT );
  pinMode( A1, OUTPUT );
  SPDR = 0x55;
  digitalWrite( A1, LOW );
  Serial.println( "SETUP COMPLETE" );
  bot->walkSquare( "f" );
}

