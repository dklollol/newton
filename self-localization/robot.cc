#include "robot.h"

void sleep(double seconds) {
  struct timespec spec;
  spec.tv_sec = (time_t) seconds;
  spec.tv_nsec = (long) ((seconds - (double) spec.tv_sec) * 10e8);
  nanosleep(&spec, NULL);
}


// drives xx cm and stops 
void drive(Position2dProxy *pp, pos_t *pos) {
  pp->SetSpeed(SPEED, DTOR(0));
  sleep(TIME+ACC_TIME);
  pp->SetSpeed(0, DTOR(0));
  pos->x = cos(pos->yaw)*SPEED*TIME;
  pos->y = sin(pos->yaw)*SPEED*TIME;
}

void turn(Position2dProxy *pp, pos_t *pos, double yaw) {
  pp->SetSpeed(0, DTOR(yaw));
  sleep(TIME+ACC_TIME);
  pp->SetSpeed(0,0);
  pos->yaw = yaw;

}
