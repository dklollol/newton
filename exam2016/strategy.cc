#include "strategy.h"

void execute_strategy(Position2dProxy &pp, pos_t &pos,
                      driving_state_t &driving_state, object::type landmark,
                      double measured_distance, double measured_angle) {
  double landmark_y;
  double landmark_x;

  decide_landmark(landmark, &landmark_x, &landmark_y);
  // Move the robot according to its current state.
  switch (driving_state) {
    case searching: {
      // The robot is turning to find the first landmark.
      puts("[STATE] searching");
      say("searching");

      turn(pp, pos, degrees_to_radians(5.0));
      break;
    }
  }
}
