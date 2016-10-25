#ifndef ROBOT_H
#define ROBOT_H

#include <libplayerc++/playerc++.h>
using namespace PlayerCc;
struct pos_t {
  double x;
  double y;
  double yaw;
};

void drive_dist(Position2dProxy *pp, pos_t *pos, double time, double speed);

#endif /* ROBOT_H */
