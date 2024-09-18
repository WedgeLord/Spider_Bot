#ifndef SPIDER_H
#define SPIDER_H

#include <Servo.h>

#include "spider_types.h"

#define H_1 2
#define H_2 3
#define H_3 4
#define H_4 5
#define F_1 6
#define F_2 7
#define F_3 8
#define F_4 9

#define LEG_RADIUS 6 // ~6cm from axis to foot
#define BODY_WIDTH 9 // 9cm from axis to axis

inline int sgn( double num ) { return ((num > 0) - (num < 0)); } // gets the sign of number ( when 0 idk?? )

// using "singleton" class structure
class Spider {

public:
  static Spider *getBot();
  void raise( int leg, int height );
  void pivot( int leg, int pivot );
  tie_return tie( tie_params params );
  void setMotor( int leg, int height, int pivot );
  void setMotor( spider_leg_t &spider_leg );
  void resetLegs();
  void walkSquare( char *direction );
  void crawl(int direction);
  void destroy();

  // no object copying allowed
  Spider(const Spider&) = delete;
  Spider& operator=(const Spider&) = delete;

private:
  struct Leg {
    int hip_pin;
    int foot_pin;
    Servo hip;
    Servo foot;    
    constraint bounds;
    // tie-related members
    Leg* tie;
    double length;
    bool clockwise;


    Leg( int h, int f, int n );
    ~Leg();
    void init_leg();
    constraint calcBounds();

  };
  static Spider* bot;
  static Leg leg[4];

  Spider();// = default;
  ~Spider() = default;
  Spider::Leg* getLeg( int index );
  void untie( uint8_t leg );

};

// initialize static members
Spider* Spider::bot{ nullptr };
Spider::Leg Spider::leg[4]{ 
  { H_1, F_1, 1 }, 
  { H_2, F_2, 2 }, 
  { H_3, F_3, 3 }, 
  { H_4, F_4, 4 }
};

Spider::Leg::Leg( int h, int f, int n ) 
  : hip_pin { h }, foot_pin { f }, tie { this }, bounds { DEFAULT_CONSTRAINT }, hip {}, foot {} {} // empty constructor body
  
Spider::Leg::~Leg() {
  hip.detach();
  foot.detach();
}      

void Spider::Leg::init_leg() {
  hip.attach( hip_pin, 600, 2500 );
  foot.attach( foot_pin, 900, 908 );
}

Spider::Leg* Spider::getLeg( int index ) { 
  if ( index < 1 || index > 4 ) {
    // Serial.println( "Invalid leg selected!" );
    return &this->leg[0];
  };
  return &this->leg[index-1];  // adjust to 0-index
}  

Spider::Spider() {
  for ( auto &leg : Spider::leg ) {
    leg.init_leg();
  }
}
 

Spider *Spider::getBot() {
  if (Spider::bot == nullptr) {
    Spider::bot = new Spider();
  }
  return Spider::bot;
}

// sets servo position of motor (1-4)
void Spider::setMotor( int leg, int height, int pivot ) {
  this->raise( leg, height );
  this->pivot( leg, pivot );
}

// adjustments to scale percentages into degrees
void Spider::raise( int leg, int height ) {
  this->getLeg( leg )->foot.write( height * 1.8 );
}

void Spider::pivot( int legNumber, int pivot ) {
  Leg* leg = this->getLeg( legNumber );
  if ( pivot < leg->bounds.lower || pivot > leg->bounds.upper ) {
    Serial.println( "out of bounds!" );
    return;
  }
  // check for tie
  Leg* tie = leg->tie;
  if ( tie == leg ) { // if not tied
    leg->hip.write( pivot );
    return;
  }    
  // if tied
  // Serial.print( "clockwise = " );
  // Serial.println( leg->clockwise ? "true" : "false" );    
  double c, length, ang;
  length = leg->length;
  if ( leg->clockwise ) {
    c = 1;
    ang = pivot - 45;
  }
  else {
    c = -1;
    ang = pivot - 135;
  }

  // Serial.print( "Angle: " );
  // Serial.println( ang );   
  // Serial.print( "pivot: " );
  // Serial.println( pivot );  
           
  ang *= PI / 180;  // convert to radians
  double b_pivot = -1*c*sgn(cos(ang)) * acos(
      sgn(cos(ang)) * ( pow( BODY_WIDTH, 2 ) - pow( length, 2 ) + 2*pow( LEG_RADIUS, 2 ) + c*2*BODY_WIDTH*LEG_RADIUS*sin(ang) ) 
      / ( 2*LEG_RADIUS * sqrt( pow( LEG_RADIUS, 2 ) + pow( BODY_WIDTH, 2 ) + c*2*BODY_WIDTH*LEG_RADIUS*sin(ang) ) )
    ) - atan( ( -c*BODY_WIDTH ) / ( LEG_RADIUS * cos(ang) ) - tan(ang) );

  b_pivot *= 180 / PI;
  if ( leg->clockwise ) {
    b_pivot += 135;
  }
  else {
    b_pivot += 45;
  }
  Serial.print( "b_pivot = " );
  Serial.println( b_pivot );    

  if ( b_pivot < 0 ) {
    // Serial.println( "Out of bounds!!!" );
    return;
  }
  
  leg->hip.write( pivot );
  tie->hip.write( b_pivot );
}

void Spider::setMotor( spider_leg_t &spider_leg ) {
  Spider::setMotor( spider_leg.leg, spider_leg.height, spider_leg.pivot );
}

void Spider::resetLegs() {
  for ( int leg = 1; leg <= 4; leg++ ) {
    Spider::setMotor( leg, 50, 90 );
  }
}

void Spider::destroy() {
  for ( Leg& l : this->leg ) {
    l.~Leg();
  }
}
constraint Spider::Leg::calcBounds() {
  constraint leg_bounds = DEFAULT_CONSTRAINT;
  if ( this == this->tie ) {
    return leg_bounds;
  }

  double s = (this->length > BODY_WIDTH) - (this->length < BODY_WIDTH);  
  double c = ( this->clockwise ) ? 1 : -1;
  double bound = asin( c*( pow( this->length, 2 ) - pow( BODY_WIDTH, 2 ) - s*2*LEG_RADIUS*this->length ) 
    / ( 2*LEG_RADIUS*BODY_WIDTH )
    );
  bound *= 180 / PI;
  if ( this->clockwise ) {
    bound += 45;
  }
  else {
    bound += 135;
  }

  if ( c == s ) {
    leg_bounds.lower = bound;
  }
  else {
    leg_bounds.upper = bound;
  }
  // Serial.print( "lower: " ); Serial.println( leg_bounds.lower );
  // Serial.print( "upper: " ); Serial.println( leg_bounds.upper );
  return leg_bounds;
}
void Spider::untie( uint8_t leg ) {
  Leg *a = this->getLeg( leg );
  Leg *b = a->tie;
  if ( a != b ) { // if leg1 is tied
    // reset ties
    a->tie = a;
    b->tie = b;
    a->bounds = DEFAULT_CONSTRAINT;
    b->bounds = DEFAULT_CONSTRAINT;
  }
}

tie_return Spider::tie( tie_params params ) {
  uint8_t leg1 = params.p[0];
  uint8_t leg2 = params.p[1];
  uint8_t pivot1 = params.p[2];
  uint8_t pivot2 = params.p[3];

  tie_return leg_bounds = { DEFAULT_CONSTRAINT, DEFAULT_CONSTRAINT };

  // Serial.println( "begin tie" );
  // check adjacency
  if ( ( leg1 - leg2 ) % 4 == 2 ) {
    Serial.println( "legs not adjacent!" );
    return leg_bounds;
  }  

  // begin untie leg1
  this->untie( leg1 );

  if ( leg1 != leg2 ) {  // legs are adjacent
    this->untie( leg2 );

    Leg *a = this->getLeg( leg1 );
    Leg *b = this->getLeg( leg2 );
    b->tie->tie = b->tie;
    double angle1 = ( pivot1 - 45 ) * PI/180;
    double angle2 = ( pivot2 - 135 ) * PI/180;

    double x1 = cos( angle1 ) * LEG_RADIUS;
    double x2 = cos( angle2 ) * LEG_RADIUS;

    double y1 = sin( angle1 ) * LEG_RADIUS;  
    double y2 = sin( angle2 ) * LEG_RADIUS;

    bool clockwise = ( ( leg1 - 1 ) % 4 == leg2 ); // leg numbers increase in counter-clockwise manner                                

    double length = sqrt( pow(  x1 - x2, 2 ) + pow( y1 - y2 + BODY_WIDTH, 2 ) );

    // Serial.print( "Tie length = " );
    // Serial.println( length );   

    // store tie data
    a->length = length;
    b->length = length; 
    a->clockwise = clockwise;
    b->clockwise = !clockwise;
    a->tie = b;
    b->tie = a;
    a->bounds = a->calcBounds(); 
    b->bounds = b->calcBounds();  
    leg_bounds = { a->bounds, b->bounds };
    this->pivot( leg1, pivot1 );  // send pivot command  
  }
  return leg_bounds;
}

void Spider::walkSquare( char *direction ) {
  switch ( direction[0] ) {
    case 'b':
      if ( this->tie( (tie_params) { 1, 2, 45, 45 } ).leg1.upper > 0 ) {
        Serial.println("success");
        for ( int i = 0; i <= 180; i += 10 ) {
          this->pivot( 3, i );
          delay( 1000 );
        }
      }
      else Serial.println("fail");
      break;

    default:
      Serial.println( "direction options are: f, b, l, r" );
  }
}

#endif