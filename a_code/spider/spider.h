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


struct Leg {

  Servo hip;
  Servo foot;

  Leg(int h, int f) {
    hip.attach(h);
    foot.attach(f);
  }
};

// using "singleton" class structure
class Spider {

public:
  static Spider& getBot();
  void crawl(int direction);
  // no object copying allowed
  Spider(const Spider&) = delete;
  Spider& operator=(const Spider&) = delete;

private:
  // static inline Spider* bot { nullptr };
  static Spider* bot;
  static Leg leg[4];

  Spider() = default;
  ~Spider() = default;
  void resetLegs();

};

// initialize static members
Spider* Spider::bot{ nullptr };
Leg Spider::leg[4]{
  { H_1, F_1 }, { H_2, F_2 }, { H_3, F_3 }, { H_4, F_4 }
};

Spider& Spider::getBot() {
  if (Spider::bot == nullptr) {
    Spider::bot = new Spider();
  }
  return *Spider::bot;
}

void Spider::resetLegs() {
  for (Leg& l : Spider::leg) {
    l.hip.write(90);
    l.foot.write(90);
  }
}

#endif