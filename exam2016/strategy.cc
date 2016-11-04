#include "strategy.h"

//string driving_state_name[] = {"searching"};

bool driven = false;
double stop_dist = 80;
int angles_to_turn = 0;
int drive_around_landmark_remaining_dist = 0;
int square_turns = 0;
void print_visited() {
  printf("landmark 1: %d\n",visited_landmarks[object::landmark1]);
  printf("landmark 2: %d\n",visited_landmarks[object::landmark2]);
  printf("landmark 3: %d\n",visited_landmarks[object::landmark3]);
  printf("landmark 4: %d\n",visited_landmarks[object::landmark4]);
}
void execute_strategy(Position2dProxy &pp, pos_t &pos,
                      driving_state_t &driving_state, object::type landmark,
                      double measured_distance, double measured_angle) {
  double landmark_y;
  double landmark_x;
  decide_landmark(landmark, &landmark_x, &landmark_y);

  string state_name = stateMap[driving_state];
  printf("[STATE] %s\n", state_name.c_str());
  say_async(state_name);
  
  // Move the robot according to its current state.
  switch (driving_state) {
  case driving_state_t::align: {
    if (landmark == object::none) {
      //   driving_state = searching_random;
      break;
    }
    else if (fabs(measured_angle) < degrees_to_radians(5.0)) {
      driving_state = approach;
    }
    else {
      turn(pp, pos, clamp(measured_angle,
                            degrees_to_radians(-5.0),
                            degrees_to_radians(5.0)));
    }
    break;
  }
  case approach: {
    // arrived at landmark! 
    if (measured_distance <= stop_dist && landmark != object::none) {
      printf("Measured distance: %f\n", measured_distance);
      turn(pp, pos, degrees_to_radians(100.0));
      drive_around_landmark_remaining_dist = 50;
      driving_state = searching_sqaure;
      square_turns = 0;
      angles_to_turn = 90;
      visited_landmarks[landmark] = true;
      print_visited();
    } else {
      drive(pp, pos,
            clamp(measured_distance - stop_dist, 0.0, 15.0));
      driving_state = driving_state_t::align; // Make sure it's still aligned.
    }
    break;
  }
  case searching_random: {
    if (landmark != object::none && !visited_landmarks[landmark]) {
      driving_state = approach;
      driven = false;
      angles_to_turn = 0;
      break;
    }
    if (!driven) {
      drive(pp, pos, 50);
      driven = true;
      break;
    }
    if (angles_to_turn == 0) {
      angles_to_turn = randf() > 0.5 ? randf()*180 : -1*randf()*180;
    }
    if (angles_to_turn < 0) {
      turn(pp, pos, degrees_to_radians(-5));
      angles_to_turn += 5;
    } else {
      turn(pp, pos, degrees_to_radians(5));
      angles_to_turn -=5;
    }
    if (abs(angles_to_turn) < 5) {
      driven = false;
      angles_to_turn = 0;
    }
    break;
  }
  case searching_sqaure: {
    if (landmark != object::none && !visited_landmarks[landmark]) {
      driving_state = approach;
      driven = false;
      angles_to_turn = 0;
      break;
    }
    if (!driven) {
      driven = true;
      drive(pp, pos, drive_around_landmark_remaining_dist);
      drive_around_landmark_remaining_dist = 110;
      break;
    }
    if (angles_to_turn == 0) {
      angles_to_turn = 90;
      // done turned and should drive next time
      driven = false;
      square_turns++;
    }
    angles_to_turn -= 5;
    turn(pp, pos, degrees_to_radians(-5));
    if (square_turns == 4) {
      driving_state = searching_random;
      angles_to_turn = 0;
    }
    break;
  }
  }
   
}
