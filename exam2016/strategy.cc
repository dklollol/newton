#include "strategy.h"

//string driving_state_name[] = {"searching"};

bool driven = false;
double stop_dist = 100;
int turned_angle = 0;
int drive_around_landmark_remaining_dist = 0;
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
  case search_turn: {
    if (landmark != object::none) {
      driving_state = approach;
      break;
    }
    turn(pp, pos, degrees_to_radians(5));
    break;
  }
  case driving_state_t::align: {
    if (landmark == object::none) {
      driving_state = search_turn;
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
      stop_dist = measured_distance;
      turn(pp, pos, degrees_to_radians(90.0));
      drive_around_landmark_remaining_dist = 50;
      driving_state = searching_sqaure;
      turned_angle = 90;
      visited_landmarks[landmark] = true;
      print_visited();
    } else {
      drive(pp, pos,
            clamp(measured_distance - stop_dist, 0.0, 15.0));
      driving_state = driving_state_t::align; // Make sure it's still aligned.
    }
    break;
  }
  case searching_random: { // redo! 
    if (landmark != object::none && !visited_landmarks[landmark]) {
      driving_state = approach;
      driven = false;
      break;
    }
    // The robot is turning to find the first landmark.
    if (!driven) {
      drive(pp, pos, 100);
      driven = true;
      break;
    }
    turned_angle -= 15;
    if (turned_angle == 0) {
      turned_angle = 90;
    }
    turn(pp, pos, degrees_to_radians(15.0));
    break;
  }
  case searching_sqaure: {
    if (landmark != object::none && !visited_landmarks[landmark]) {
      driving_state = approach;
      driven = false;
      break;
    }
    if (!driven) {
      driven = true;
      drive(pp, pos, drive_around_landmark_remaining_dist);
      drive_around_landmark_remaining_dist = 130;
      break;
    }
    turned_angle -= 5;
    if (turned_angle == 0) {
      turned_angle = 90;
      // done turned and should drive next time
      driven = false;
    }
      turn(pp, pos, degrees_to_radians(-5));

  }
  }
}
