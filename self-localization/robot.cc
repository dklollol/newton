#include <cmath>
#include "robot.h"
#include "particles.h"
#include "misc.h"


void turn(Position2dProxy *pp, pos_t *pos, double turn_rad) {
  const double turn_speed = degrees_to_radians(45);
  pp->SetSpeed(0.0, turn_speed);
  sleep(fabs(turn_rad / turn_speed));
  pp->SetSpeed(0.0, 0.0);
  pos->turn += turn_rad;
}

void drive(Position2dProxy *pp, pos_t *pos, double dist_cm) {
  const double speed_cm = 20.0;
  pp->SetSpeed(dist_cm / 100.0, 0.0);
  sleep(fabs(dist_cm / speed_cm));
  pp->SetSpeed(0.0, 0.0);
  pos->x = pos->x + dist_cm * cos(pos->turn);
  pos->y = pos->y + dist_cm * sin(pos->turn);
}
