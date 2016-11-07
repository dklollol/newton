#include "strategy.h"


#define GOTO(driving_state_new) \
  printf("[STATE CHANGE] %s -> %s\n", stateMap[driving_state].c_str(), stateMap[driving_state_new].c_str()); \
  driving_state = driving_state_new;


bool driven = false;
double stop_dist = 80;
double angles_to_turn = 0;
int drive_around_landmark_remaining_dist = 0;
int square_turns = 0;
void print_landmark_status() {
  printf("Seen landmark 1? %d and visited?: %d\n", seen_landmarks[object::landmark1],
         visited_landmarks[object::landmark1]);
  printf("Seen landmark 2? %d and visited?: %d\n", seen_landmarks[object::landmark2],
         visited_landmarks[object::landmark2]);
  printf("Seen landmark 3? %d and visited?: %d\n", seen_landmarks[object::landmark3],
         visited_landmarks[object::landmark3]);
  printf("Seen landmark 4? %d and visited?: %d\n", seen_landmarks[object::landmark4],
         visited_landmarks[object::landmark4]);
}

object::type next_landmark() {
  for (auto lm : visited_landmarks) {
    if (!lm.second) {
      printf("whuhu found next landmark");
      return lm.first;
    }
  }
  // should not happen but never knows!
  return object::none;
}

bool particle_filter_usable() {
  int i = 0;
  for (auto& landmark : seen_landmarks) {
    if (landmark.second) {
      i++;
    }
  }
  return i >= 2;
}
void execute_strategy(Position2dProxy &pp, pos_t &pos, particle &p,
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
  case goto_landmark: {
    double x;
    double y;
    object::type n_landmark = next_landmark();
    decide_landmark(n_landmark, &x, &y);
    printf("Next landmark is : %s, located at: (%f,%f)\n", object::name(n_landmark).c_str(),
           x , y);
    double angle = atan2(x-p.x, y-p.y) - p.theta;
    double dist = sqrt(pow(x-p.x, 2.0) + pow(y-p.y, 2.0));
    printf("We should turn : %f degress\n", radians_to_degrees(angle));
    printf("we should drive : %f cm\n", dist);
    turn(pp, pos, angle);
    drive(pp, pos, dist);
    GOTO(finished)
    break;
  }
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
    // printf("Can i use particle filter? %d\n", particle_filter_usable());
    if (particle_filter_usable()) {
      GOTO(goto_landmark);
      break;
    }
    // arrived at landmark! 
    if (measured_distance <= stop_dist && landmark == next_landmark()) {
      printf("Measured distance: %f\n", measured_distance);
      turn(pp, pos, degrees_to_radians(100.0));
      drive_around_landmark_remaining_dist = 50;
      GOTO(searching_sqaure);
      square_turns = 0;
      angles_to_turn = 90;
      visited_landmarks[landmark] = true;
      print_landmark_status();
    } else {
      drive(pp, pos,
            clamp(measured_distance - stop_dist, 0.0, 15.0));
      GOTO(driving_state_t::align); // Make sure it's still aligned.
    }
    break;
  }
  case searching_random: {
    if (landmark != object::none) {
      seen_landmarks[landmark] = true;
      //print_seen();
      if (landmark == next_landmark()) {
        GOTO(approach);
        driven = false;
        angles_to_turn = 0;
        break;
      }
    }
    if (!driven) {
      drive(pp, pos, 50);
      driven = true;
      break;
    }
    if (angles_to_turn == 0) {
      angles_to_turn = randf() > 0.5 ? randf()*180 : -1*randf()*180;
    }
    /*
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
    */

    if (handle_turning(pp, pos, angles_to_turn, degrees_to_radians(5))) {
      driven = false;
    }
    break;
  }
  case searching_sqaure: {
    if (landmark != object::none) {
      seen_landmarks[landmark] = true;
      //print_seen();
      if (landmark == next_landmark()) {
        GOTO(approach);
        driven = false;
        angles_to_turn = 0;
        break;
      }
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
    /*
    angles_to_turn -= 5;
    turn(pp, pos, degrees_to_radians(-5));
    */

    handle_turning(pp, pos, angles_to_turn, degrees_to_radians(5));

    if (square_turns == 4) {
      GOTO(searching_random);
      angles_to_turn = 0;
      driven = true;
    }
    break;
  }
  }
   
}
