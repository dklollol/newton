#ifndef EX5_DRAW_H
#define EX5_DRAW_H

#include "particles.h"

using namespace cv;
using namespace std;

void draw_world (particle &est_pose, vector<particle> &particles, Mat &im);

#endif
