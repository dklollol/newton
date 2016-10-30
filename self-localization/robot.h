#ifndef EX5_ROBOT_H
#define EX5_ROBOT_H

#include <libplayerc++/playerc++.h>

#include "particles.h"


using namespace PlayerCc;

struct pos_t {
  double speed;
  double turn;
};

void drive(Position2dProxy *pp, pos_t *pos, double speed);

void turn(Position2dProxy *pp, pos_t *pos, double yaw);

void driveturn(Position2dProxy *pp, pos_t *pos, double speed, double yaw);

void gotoPos(Position2dProxy *pp, particle *p, double x, double y);


#endif
