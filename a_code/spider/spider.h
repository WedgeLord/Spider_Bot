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
      : hip_pin { h }, foot_pin { f }, hip {}, foot {} {
        Serial.println( "leg created" );
      } // empty constructor body
      
    ~Leg() {
      hip.detach();
      foot.detach();
      Serial.println( "leg destroyed" );
    }      

    init_leg() {
      hip.attach( hip_pin, 700, 2200 );
      foot.attach( foot_pin, 900, 910 );
      // hip.write( 90 );
      // foot.write( 90 );
    }

  };
// static inline Spider* bot { nullptr };
  static Spider* bot;
  static Leg leg[4];

  Spider();// = default;
  ~Spider() = default;

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
    Serial.println("leg init'd");
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
    // adjustments to scale percentages into degrees
    this->leg[leg-1].foot.write( height * 1.8 );
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

void Spider::walkSquare( char *direction ) {
  switch ( direction[0] ) {
    case 'f': 
      this->setMotor( 2, 100, 50 );
      this->setMotor( 3, 100, 50 );
      this->setMotor( 1, 0, 50 );
      this->setMotor( 4, 0, 50 );
      delay( 1000 );
      this->setMotor( 3, 100, 100 );
      delay( 1000 );
      this->setMotor( 3, 0, 100 );
      this->setMotor( 2, 0, 50 );
      delay( 1000 );
      this->setMotor( 1, 100, 50 );
      this->setMotor( 4, 100, 50 );
      delay( 1000 );
      this->setMotor( 1, 100, 75 );
      delay( 1000 );
      this->setMotor( 4, 0, 50 );
      
      break;
    default:
      Serial.println( "direction options are: f, b, l, r" );
  }
}

#endif