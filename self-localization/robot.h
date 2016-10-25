#ifndef ROBOT_H
#define ROBOT_H

#include <libplayerc++/playerc++.h>
const double TIME = 1.0;
const double SPEED = 1;
const double ACC_TIME = SPEED*pow(10, -6);


using namespace PlayerCc;
struct pos_t {
  double x;
  double y;
  double yaw;
};

void drive(Position2dProxy *pp, pos_t *pos);

void turn(Position2dProxy *pp, pos_t *pos, double yaw);


#endif /* ROBOT_H */
