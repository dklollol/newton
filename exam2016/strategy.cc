#include "strategy.h"

char* driving_state_name[] = {"searching"};

void execute_strategy(Position2dProxy &pp, pos_t &pos,
                      driving_state_t &driving_state, object::type landmark,
                      double measured_distance, double measured_angle) {
  double landmark_y;
  double landmark_x;
  decide_landmark(landmark, &landmark_x, &landmark_y);

  char* state_name = driving_state_name[driving_state];
  printf("[STATE] %s\n", state_name);
  say(state_name);
  
  // Move the robot according to its current state.
  switch (driving_state) {
    case searching: {
      // The robot is turning to find the first landmark.

      turn(pp, pos, degrees_to_radians(5.0));
      break;
    }
  }
}
