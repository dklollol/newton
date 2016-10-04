#include "calibration.h"


void sleep(double seconds) {
  struct timespec spec;
  spec.tv_sec = (time_t) seconds;
  spec.tv_nsec = (long) ((seconds - (double) spec.tv_sec) * 10e8);
  nanosleep(&spec, NULL);
}


// drives xx cm and stops 
void drive_dist(Position2dProxy pp, double dist, double speed) {
  double acc_time = speed*pow(10, -6);
  double time = dist / speed;
  pp.SetSpeed(speed, DTOR(0));
  sleep(time+acc_time);
  pp.SetSpeed(0, DTOR(0));
}
