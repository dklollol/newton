#ifndef ROBOT_H
#define ROBOT_H

#include <libplayerc++/playerc++.h>
#include "particles.h"

/*
const double TIME = 0.3;
const double SPEED = 0.1;
*/

using namespace PlayerCc;
struct pos_t {
  double speed;
  double turn;
};

void drive(Position2dProxy *pp, pos_t *pos, double speed);

void turn(Position2dProxy *pp, pos_t *pos, double yaw);

/*
void drive_particle(particle &p, pos_t *pos);

void turn_particle(pos_t *pos,  double yaw);
*/

void gotoPos(Position2dProxy *pp, particle *p, double x, double y);


#endif /* ROBOT_H */
