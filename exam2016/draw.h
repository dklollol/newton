#ifndef EXAM2016_DRAW_H
#define EXAM2016_DRAW_H

#include "particles.h"

using namespace cv;
using namespace std;

void draw_world (particle &est_pose, vector<particle> &particles, Mat &im);

#endif
