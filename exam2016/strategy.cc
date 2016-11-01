#include "strategy.h"

void say(string text) {
  std::system((string("espeak '") + string(text) + string("' &")).c_str());
  return;
}

void execute_strategy(Position2dProxy *pp, pos_t *pos, state *robot_state, object::type landmark, double measured_angle, double measured_distance) {
  // Move the robot according to its current state.
  switch (*robot_state) {
    case searching: {
      puts("searching");
      say("searching");
      // The robot is turning to find the first landmark.
      
      turn(pp, pos, degrees_to_radians(5.0));
      break;
    }
  }
}
