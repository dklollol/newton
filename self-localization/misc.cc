#include <math.h>
#include <algorithm>

#include "misc.h"


double degrees_to_radians(double degrees) {
  return degrees * M_PI / 180.0;
}

double radians_to_degrees(double radians) {
  return radians * 180.0 / M_PI;
}

float clamp(float n, float lower, float upper) {
  return std::max(lower, std::min(n, upper));
}
