#ifndef ROBOT_H
#define ROBOT_H

#include <libplayerc++/playerc++.h>
#include "particles.h"
const double TIME = 0.3;
const double SPEED = 0.1;


using namespace PlayerCc;
struct pos_t {
  double x;
  double y;
  double yaw;
};

void drive(Position2dProxy *pp);

void drive_particle(particle &p , pos_t *pos);

void turn(Position2dProxy *pp, double yaw);


#endif /* ROBOT_H */
