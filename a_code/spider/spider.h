#ifndef SPIDER_H
#define SPIDER_H

#include <Servo.h>

#define H_1 2
#define H_2 3
#define H_3 4
#define H_4 5
#define F_1 6
#define F_2 7
#define F_3 8
#define F_4 9

typedef struct {
  int32_t leg;
  int32_t height;
  int32_t pivot;
} spider_leg_t;

// using "singleton" class structure
class Spider {

public:
  static Spider *getBot();
  void raise( int leg, int height );
  void pivot( int leg, int pivot );
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

    Leg( int h, int f ) 
      : hip_pin { h }, foot_pin { f }, hip {}, foot {} {} // empty constructor body
      
    ~Leg() {
      hip.detach();
      foot.detach();
    }      

    init_leg() {
      hip.attach( hip_pin, 600, 2500 );
      foot.attach( foot_pin, 900, 908 );
      // hip.write( 90 );
      // foot.write( 90 );
    }

  };
// static inline Spider* bot { nullptr };
  static Spider* bot;
  static Leg leg[4];

  Spider();// = default;
  ~Spider() = default;
  tie( int leg1, int leg2, int angle1, int angle2 );

};

// initialize static members
Spider* Spider::bot{ nullptr };
Spider::Leg Spider::leg[4]{ 
  { H_1, F_1 }, 
  { H_2, F_2 }, 
  { H_3, F_3 }, 
  { H_4, F_4 }
};

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
    this->leg[leg-1].foot.write( height * 1.8 );
}
void Spider::pivot( int leg, int pivot ) {
    this->leg[leg-1].hip.write( (100 - pivot) * 1.8 );
}

void Spider::setMotor( spider_leg_t &spider_leg ) {
  Spider::setMotor( spider_leg.leg, spider_leg.height, spider_leg.pivot );
}

void Spider::resetLegs() {
  for ( int leg = 1; leg <= 4; leg++ ) {
    Spider::setMotor( leg, 50, 50 );
  }
}

void Spider::destroy() {
  for ( Leg l : this->leg ) {
    l.~Leg();
  }
}

void Spider::tie( int leg1, int leg2, int angle1, int angle2 ) {
  
}

void Spider::walkSquare( char *direction ) {
  switch ( direction[0] ) {
    case 'f': 
      this->setMotor( 1, 0, 50 );
      this->setMotor( 2, 0, 65 );
      this->setMotor( 3, 0, 50 );
      this->setMotor( 4, 0, 50 );
      delay( 1000 );
      this->pivot( 1, 40 );
      this->pivot( 2, 55 );
      delay( 1000 );
      this->pivot( 1, 34 );
      this->pivot( 2, 45 );
      delay( 1000 );
      this->setMotor( 3, 20, 25 );
      this->setMotor( 4, 50, 75 );
      delay( 1000 );
      this->pivot( 1, 33 );
      this->pivot( 2, 35 );
      delay( 1000 );
      this->pivot( 1, 36 );
      this->pivot( 2, 25 );
      delay( 1000 );
      this->pivot( 1, 43 );
      this->pivot( 2, 15 );
      delay( 1000 );
      this->pivot( 1, 60 );
      this->pivot( 2, 0 );
      break;
    case 't':
      this->setMotor( 1, 50, 100 );
      this->setMotor( 2, 50, 98 );
      this->setMotor( 3, 50, 50 );
      this->setMotor( 4, 0, 50 );
      delay( 100 );
      this->pivot( 1, 90 );
      this->pivot( 2, 96 );
      delay( 100 );
      this->pivot( 1, 80 );
      this->pivot( 2, 94 );
      delay( 100 );
      this->pivot( 1, 70 );
      this->pivot( 2, 89 );
      delay( 100 );
      this->pivot( 1, 60 );
      this->pivot( 2, 84 );
      delay( 100 );
      this->pivot( 1, 50 );
      this->pivot( 2, 77 );
      delay( 100 );
      this->pivot( 1, 40 );
      this->pivot( 2, 68 );
      delay( 100 );
      this->pivot( 1, 30 );
      this->pivot( 2, 58 );
      delay( 100 );
      this->pivot( 1, 20 );
      this->pivot( 2, 47 );
      delay( 100 );
      this->pivot( 1, 10 );
      this->pivot( 2, 30 );
      /* a little unnecessary
      delay( 100 );
      this->pivot( 1, 6 );
      this->pivot( 2, 20 );
      delay( 100 );
      this->pivot( 1, 3 );
      this->pivot( 2, 4 );
      */
      delay( 100 );
      break;
    case 's':
      this->setMotor( 1, 0, 0 );
      this->setMotor( 2, 0, 0 );
      this->setMotor( 3, 0, 0 );
      this->setMotor( 4, 0, 0 );
      delay( 5000 );
      this->pivot( 1, 100 );
      this->pivot( 2, 100 );
      this->pivot( 3, 100 );
      this->pivot( 4, 100 );
      delay( 5000 );
      break;
    default:
      Serial.println( "direction options are: f, b, l, r" );
  }
}

#endif