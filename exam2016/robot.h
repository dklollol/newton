#ifndef EXAM2016_ROBOT_H
#define EXAM2016_ROBOT_H

#include <libplayerc++/playerc++.h>

#include "particles.h"

using namespace PlayerCc;


struct pos_t {
  double x;
  double y;
  double turn;
};

void turn(Position2dProxy &pp, pos_t &pos, double turn_rad);

void drive(PlayerClient &robot, Position2dProxy &pp, IrProxy &ir,
           pos_t &pos, double dist_cm);

bool handle_turning(Position2dProxy &pp, pos_t &pos, double &angle_var, double turn_rad);

float ir_correction(int index, IrProxy &ir);

bool check_sensor(int index, float threshold, IrProxy &ir);

bool check_sensors(PlayerClient &robot, Position2dProxy &pp, IrProxy &ir, double threshold);

double check_right(IrProxy &ir);

double check_left(IrProxy &ir);

#endif
