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
float ir_correction(float range) {
  return range * 0.93569 - 0.103488;
}

bool check_sensor(int index, float threshold, IrProxy &ir) {  
  return (ir_correction(ir.GetRange(index)) < threshold);
}

void center_robot_green_box(Position2dProxy pp, Box box) {
  double resolution = 640;
  while(true) {
    double diff = ((resolution / 2) - box.center.x) / (resolution / 2);
		
    pp.SetSpeed(0, DTOR(20 * diff));

    if (fabs(diff) < 0.1) {
      return;
    }
  }
  
}
