#include "robot.h"
#include "particles.h"


// drives xx cm and stops 
void drive(Position2dProxy *pp, pos_t *pos, double speed) {
  pp->SetSpeed(speed / 100, 0.0);
  pos->speed = speed * (5.0 / 1000.0);
}

// yaw is radians! 
void turn(Position2dProxy *pp, pos_t *pos, double yaw) {
  pp->SetSpeed(0.0, DTOR(yaw));
  pos->turn = DTOR(yaw) * (5.0 / 1000.0);
}

// both drive and turn
void driveturn(Position2dProxy *pp, pos_t *pos, double speed, double yaw) {
  pp->SetSpeed(speed / 100, DTOR(yaw));
  pos->speed = speed * (5.0 / 1000.0);
  pos->turn = DTOR(yaw) * (5.0 / 1000.0);
}

void gotoPos(Position2dProxy *pp, particle *p, double x, double y) {
  return;
}
