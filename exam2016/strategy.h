#include "robot.h"
#include "misc.h"
#include <libplayerc++/playerc++.h>

enum state {searching, align, approach,
            drive_around_landmark,
            triangulating,
            drive_to_center, arrived_at_center};
            
void say(string text);


void execute_strategy(Position2dProxy *pp, pos_t *pos, state *robot_state,
                      object::type landmark, double measured_angle, double measured_distance);
