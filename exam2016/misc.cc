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
