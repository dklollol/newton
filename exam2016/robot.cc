#include <cmath>
#include "robot.h"
#include "particles.h"
#include "misc.h"


void turn(Position2dProxy &pp, pos_t &pos, double turn_rad) {
  const double turn_speed = degrees_to_radians(20);
  pp.SetSpeed(0.0, turn_speed * ((turn_rad >= 0) ? 1 : -1));
  sleep(fabs(turn_rad / turn_speed));
  //sleep(0.3);
  pp.SetSpeed(0.0, 0.0);
  pos.turn += turn_rad;
}

void drive(Position2dProxy &pp, pos_t &pos, double dist_cm) {
  const double speed_cm = 20.0;
  pp.SetSpeed(speed_cm / 100.0, 0.0);
  sleep(fabs(dist_cm / speed_cm));
  pp.SetSpeed(0.0, 0.0);
  pos.x = pos.x + dist_cm * cos(pos.turn);
  pos.y = pos.y + dist_cm * sin(pos.turn);
}

bool handle_turning(Position2dProxy &pp, pos_t &pos, double &angle_var, double turn_rad) {
  if (angle_var < 0) {
    turn(pp, pos, -turn_rad);
    angle_var += radians_to_degrees(turn_rad);
    } else {
    turn(pp, pos, turn_rad);
    angle_var -= radians_to_degrees(turn_rad);
  }
  if (abs(angle_var) < radians_to_degrees(turn_rad)) {
    angle_var = 0;
    return true;
    } else {
    return false;
  }
}
