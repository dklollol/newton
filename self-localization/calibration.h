#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <libplayerc++/playerc++.h>     // Robot interaction
#include <cmath>
#include "cam.h"

using namespace PlayerCc;


bool center_robot_green_box(VideoCapture cam, Position2dProxy *pp, Box *box);
  
void sleep(double seconds);

void drive_dist(Position2dProxy *pp, double dist, double speed);

float ir_correction(float range);

bool check_sensor(int index, float threshold, IrProxy &ir);

double find_fov(VideoCapture cam, Box *box, double known_dist, double known_height);

#endif
