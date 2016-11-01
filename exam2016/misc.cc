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

double driveVariance(double x, double y) {
  if (x+y == 0 ) {
    return 0;
  }
  return sqrt((x*x + y*y)/(x+y));
}

double turnVariance(double theta) {
  return theta/10;
}
