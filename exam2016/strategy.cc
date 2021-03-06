#include "strategy.h"
#include "robot.h"


#define GOTO(driving_state_new) \
printf("[STATE CHANGE] %s -> %s\n", stateMap[driving_state].c_str(), stateMap[driving_state_new].c_str()); \
driving_state = driving_state_new;


bool driven = false;
double stop_dist = 75;
double angles_to_turn = 0;
int drive_around_landmark_remaining_dist = 0;
int square_turns = 0;
int wait_for_landmark = 0;
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

void execute_strategy(PlayerClient &robot,
Position2dProxy &pp, IrProxy &ir, pos_t &pos, particle &p,
driving_state_t &driving_state, object::type landmark,
double measured_distance, double measured_angle) {
  string state_name = stateMap[driving_state];
  printf("[STATE] %s\n", state_name.c_str());
  say_sync(state_name);
  
  // Move the robot according to its current state.
  switch (driving_state) {
    
    case goto_landmark: {
      double x; double y;
      object::type n_landmark = next_landmark();
      decide_landmark(n_landmark, &x, &y);
      printf("Next landmark is : %s, located at: (%f,%f)\n", object::name(n_landmark).c_str(),
      x , y);
      double angle = atan2(y-p.y, x-p.y) - p.theta;
      double dist = sqrt(pow(x-p.x, 2.0) + pow(y-p.y, 2.0)) - stop_dist;
      printf("We should turn : %f degress\n", radians_to_degrees(angle));
      printf("we should drive : %f cm\n", dist);
      turn(pp, pos, angle);
      drive(robot, pp, ir, pos, dist, true);
      
      //visited_landmarks[n_landmark] = true;
      if (n_landmark == object::landmark4) {
        GOTO(finished);
      }
      else {
        GOTO(approach);
      }
      break;
    }
    
    
    /* APPROACH AND ALIGN */
    case approach: {
      // arrived at landmark!
      if (measured_distance <= stop_dist && landmark == next_landmark()) {
        visited_landmarks[landmark] = true;
        print_landmark_status();
        if (landmark != object::landmark1) {
          GOTO(goto_landmark);
          break;
        }
        turn(pp, pos, degrees_to_radians(92.0));
        drive_around_landmark_remaining_dist = 50;
        GOTO(searching_sqaure);
        square_turns = 0;
        angles_to_turn = -85;
        break;
        }
      if (landmark != next_landmark() && particle_filter_usable() && landmark != object::none) {
        GOTO(goto_landmark);
        break;
      }
        drive(robot, pp, ir, pos,
        clamp(measured_distance - stop_dist, 0.0, 15.0), true);
        GOTO(driving_state_t::align); // Make sure it's still aligned.
        break;
    }
    
    case driving_state_t::align: {
      if (landmark != next_landmark()) {
        if (particle_filter_usable()) {
          GOTO(goto_landmark);
        }
        break;
      }
      if (fabs(measured_angle) < degrees_to_radians(5.0)) {
        GOTO(approach);
      } else {
        printf("I should turn in align!");
        turn(pp, pos, clamp(measured_angle,
        degrees_to_radians(-5.0),
        degrees_to_radians(5.0)));
      }
      break;
    }
    
    
    /* SEARCHING STATES */
    case searching_random: {
      if (landmark != object::none) {
        seen_landmarks[landmark] = true;
        if (landmark == next_landmark()) {
          GOTO(approach);
          driven = false;
          //angles_to_turn = 0;
          break;
        }
      }
      if (!driven) {
        drive(robot, pp, ir, pos, 50, true);
        driven = true;
        break;
      }
      if (angles_to_turn == 0) {
        angles_to_turn = randf() > 0.5 ? randf()*180 : -1*randf()*180;
      }
      if (handle_turning(pp, pos, angles_to_turn, degrees_to_radians(5))) {
        angles_to_turn = 0;
        driven = false;
      }
      break;
    }
    
    case searching_sqaure: {
      if (landmark != object::none) {
        seen_landmarks[landmark] = true;
        if (landmark == next_landmark()) {
          GOTO(approach);
          driven = false;
          angles_to_turn = 0;
          break;
        }
      }
      if (!driven) {
        driven = true;
        drive(robot, pp, ir, pos, drive_around_landmark_remaining_dist, false);
        drive_around_landmark_remaining_dist = 110;
        break;
      }
      if (handle_turning(pp, pos, angles_to_turn, degrees_to_radians(5))) {
        printf("I've turned %f degrees\n", 90 + angles_to_turn);
        angles_to_turn = -85;
        // done turned and should drive next time
        driven = false;
        square_turns++;
      }
      
      if (square_turns == 4) {
        GOTO(searching_random);
        angles_to_turn = 0;
        driven = true;
      }
      break;
    }
    
    
    /* THE END */
    case finished: {
      puts("Finish (but this will not be printed).");
      break;
    }

    /* Testing for our particle filter */
    case test_search: {
      if (landmark != object::none && wait_for_landmark != 0) {
        seen_landmarks[landmark] = true;
        wait_for_landmark --;
        break;
      }
      if (angles_to_turn == 0) {
        angles_to_turn = 360;
      }
      if (particle_filter_usable()) {
        GOTO(test_goto);
      }
      if (handle_turning(pp, pos, angles_to_turn, degrees_to_radians(5))) {
        GOTO(test_goto);
      }
      wait_for_landmark = 20;
      break;
    }

    case test_goto: {
      double x; double y;
      object::type n_landmark = next_landmark();
      decide_landmark(n_landmark, &x, &y);
      printf("Next landmark is : %s, located at: (%f,%f)\n", object::name(n_landmark).c_str(),
      x , y);
      double angle = atan2(y-p.y, x-p.y) - p.theta;
      double dist = sqrt(pow(x-p.x, 2.0) + pow(y-p.y, 2.0)) - stop_dist;
      printf("We should turn : %f degress\n", radians_to_degrees(angle));
      printf("we should drive : %f cm\n", dist);
      turn(pp, pos, angle);
      drive(robot, pp, ir, pos, dist, false);
      
      visited_landmarks[n_landmark] = true;
      if (n_landmark == object::landmark4) {
        GOTO(finished);
      }
      break;
    }
  } 
}
