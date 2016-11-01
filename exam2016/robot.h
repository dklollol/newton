#ifndef EX5_ROBOT_H
#define EX5_ROBOT_H

#include <libplayerc++/playerc++.h>

#include "particles.h"


using namespace PlayerCc;

struct pos_t {
  double x;
  double y;
  double turn;
};

void turn(Position2dProxy *pp, pos_t *pos, double turn_rad);

void drive(Position2dProxy *pp, pos_t *pos, double dist_cm);

#endif
