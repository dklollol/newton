#include "strategy.h"

void say(string text) {
  std::system((string("espeak '") + string(text) + string("' &")).c_str());
  return;
}

void execute_strategy(Position2dProxy *pp, pos_t *pos, state *robot_state, object::type landmark, double measured_angle, double measured_distance) {
  double landmark_y;
  double landmark_x;

  switch (landmark_id) {
    case object::Landmark_1: {
      landmark_x = 0;
      landmark_y = 300;
      break;
    }
    case object::Landmark_2: {
      landmark_x = 0;
      landmark_y = 0;
      break;
    }
    case object::Landmark_3: {
      landmark_x = 300;
      landmark_y = 300;
      break;
    }
    case object::Landmark_4: {
      landmark_x = 300;
      landmark_y = 0;
      break;
    }
  }

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
