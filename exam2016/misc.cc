#include <algorithm>
#include <cmath>
#include <ctime>

#include "misc.h"


double degrees_to_radians(double degrees) {
  return degrees * M_PI / 180.0;
}

double radians_to_degrees(double radians) {
  return radians * 180.0 / M_PI;
}

double clamp(double n, double lower, double upper) {
  return std::max(lower, std::min(n, upper));
}

void sleep(double seconds) {
  struct timespec spec;
  spec.tv_sec = (time_t) seconds;
  spec.tv_nsec = (long) ((seconds - (double) spec.tv_sec) * 10e8);
  nanosleep(&spec, NULL);
}

double distance(double x1, double x2, double y1, double y2) {
  return sqrt(pow(x1 - x2, 2.0) + pow(y1 - y2, 2.0));
}

tuple<double, double> command_variance(pos_t *pos) {
  double x = pos->x;
  double y = pos->y;
  double theta = pos->turn;
  if (x == 0 && y == 0 && theta == 0) {
    return make_tuple(2, degrees_to_radians(2));
  }
  if (x+y == 0) {
    return make_tuple(1, theta/10);
  }
  return make_tuple(sqrt(fabs(x)+fabs(y)), degrees_to_radians(1));
}

void decide_landmark(object::type landmark_id, double *x, double *y){
  switch (landmark_id) {
  case object::landmark1: {
    *x = 0;
    *y = 300;
    break;
  }
  case object::landmark2: {
    *x = 0;
    *y = 0;
    break;
  }
  case object::landmark3: {
    *x = 300;
    *y = 300;
    break;
  }
  case object::landmark4: {
    *x = 300;
    *y = 0;
    break;
  }
  default: {
    break;
  }
  }
}

void say_sync(string text) {
  std::system((string("espeak '")
               + string(text)
               + string("'")).c_str());
}

void say_async(string text) {
  std::system((string("espeak '")
               + string(text)
               + string("' &")).c_str());
}
