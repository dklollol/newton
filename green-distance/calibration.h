#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <libplayerc++/playerc++.h>     // Robot interaction
#include <cmath>
using namespace PlayerCc;

void sleep(double seconds);

void drive_dist(Position2dProxy pp, double dist, double speed);

#endif
