#include "calibration.h"


void sleep(double seconds) {
  struct timespec spec;
  spec.tv_sec = (time_t) seconds;
  spec.tv_nsec = (long) ((seconds - (double) spec.tv_sec) * 10e8);
  nanosleep(&spec, NULL);
}


// drives xx cm and stops 
void drive_dist(Position2dProxy *pp, double dist, double speed) {
  double acc_time = speed*pow(10, -6);
  double time = (dist / speed)/100;
  pp->SetSpeed(speed, DTOR(0));
  sleep(time+acc_time);
  pp->SetSpeed(0, DTOR(0));
}
float ir_correction(float range) {
  return range * 0.93569 - 0.103488;
}

bool check_sensor(int index, float threshold, IrProxy &ir) {  
  return (ir_correction(ir.GetRange(index)) < threshold);
}

bool center_robot_green_box(VideoCapture cam, Position2dProxy *pp, Box *box) {
  if (!(box->found)) {
    return false;
  }
  double resolution = 640;
  Box box1;
  while(true) {
    double diff = ((resolution / 2) - box->center.x) / (resolution / 2);
    if (fabs(diff) < 0.1) {
      pp->SetSpeed(0, DTOR(0));
      return true;
    }
    pp->SetSpeed(0, DTOR(20 * diff));
    box1 = get_box(cam);
    box = &box1;
  }
}

double find_fov(VideoCapture cam, Box *box, double known_dist, double known_height) {
  double total_height = (get_cam_height(cam) / box->height) * known_height;
  double a = total_height / 2;
  double b = known_dist;
  double c = sqrt(a * a + b * b);
  double A = asin(a / c);
  double fov = radians_to_degrees(A * 2.0);
  return fov;
}
