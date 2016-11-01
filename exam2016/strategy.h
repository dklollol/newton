#ifndef EXAM2016_STRATEGY_H
#define EXAM2016_STRATEGY_H

#include <libplayerc++/playerc++.h>

#include "robot.h"
#include "misc.h"
#include "camera.h"


enum driving_state_t {searching};

void execute_strategy(Position2dProxy &pp, pos_t &pos,
                      driving_state_t &driving_state, object::type landmark,
                      double measured_distance, double measured_angle);

#endif
