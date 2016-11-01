#ifndef EXAM2016_MISC_H
#define EXAM2016_MISC_H
#include <tuple>
#include "robot.h"
using namespace std;
double degrees_to_radians(double degrees);

double radians_to_degrees(double radians);

double clamp(double n, double lower, double upper);

void decide_landmark(object::type landmark, double *x, double *y);

void sleep(double seconds);

double distance(double x1, double x2, double y1, double y2);

tuple<double, double> command_variance(pos_t *pos);

void say(string text);


#endif
