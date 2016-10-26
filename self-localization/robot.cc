#include "robot.h"
#include "particles.h"

void sleep(double seconds) {
  struct timespec spec;
  spec.tv_sec = (time_t) seconds;
  spec.tv_nsec = (long) ((seconds - (double) spec.tv_sec) * 10e8);
  nanosleep(&spec, NULL);
}


// drives xx cm and stops 
void drive(Position2dProxy *pp) {
  pp->SetSpeed(SPEED, DTOR(0));
  sleep(TIME);
  pp->SetSpeed(0, DTOR(0));
}

void drive_particle(particle &p, pos_t *pos) {
  pos->x = cos(p.theta)*SPEED*(TIME)*100;
  pos->y = sin(p.theta)*SPEED*(TIME)*100;
  pos->yaw = 0;
}
// yaw is radians! 
void turn(Position2dProxy *pp, double yaw) {
  pp->SetSpeed(0, yaw);
  sleep(TIME);
  pp->SetSpeed(0,0);
}

void turn_particle(pos_t *pos, double yaw) {
  pos->x = 0;
  pos->y = 0;
  pos->yaw = yaw;
}


