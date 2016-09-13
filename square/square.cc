#include <iostream>
#include <libplayerc++/playerc++.h>
using namespace PlayerCc;

void run_square(char* host, int port, int device_index) {
  PlayerClient robot(host, port);
  Position2dProxy pp(&robot, device_index);
  // KØR 1 METER FREMAD!
  double move_init = 0.2;
  double move_turn_init = 0.0;
  timespec move_sleep_init = {0, 100000000};

  pp.SetSpeed(move_init, DTOR(move_turn_init));
  nanosleep(&move_sleep_init, NULL);

  double move_speed;
  double move_turn_rate;
  timespec move_sleep;
  double turn_speed;
  double turn_turn_rate;
  timespec turn_sleep;

    
  for (int t = 0; t < 4; t++) {

    // GØR DET FØLGENDE FIRE GANGE!
    for (int i = 0; i < 4; i++) {
      // KØR 1 METER FREMAD!
      move_speed = 0.2;
      move_turn_rate = 0.0;
      move_sleep = {5, 0};

      pp.SetSpeed(move_speed, DTOR(move_turn_rate));
      nanosleep(&move_sleep, NULL);

      // DREJ 90 GRADER!
      turn_speed = 0.0;
      turn_turn_rate = 14.7;
      turn_sleep = {6, 0};

      pp.SetSpeed(turn_speed, DTOR(turn_turn_rate));
      nanosleep(&turn_sleep, NULL);
    }
  }
  // STOP!
  timespec stop_sleep = {1, 0};
  pp.SetSpeed(0, 0);
  nanosleep(&stop_sleep, NULL);
}

int main(int argc, char* argv[]) {
  char* host;
  if (argc > 1) {
    host = argv[1];
  }
  else {
    host = (char*) "localhost";
  }

  const int port = 6665;
  const int device_index = 0;
  try {
    run_square(host, port, device_index);
    return EXIT_SUCCESS;
  } catch (PlayerCc::PlayerError e) {
    std::cerr << e << std::endl;
    return EXIT_FAILURE;
  }
}
