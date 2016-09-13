#include <iostream>
#include <libplayerc++/playerc++.h>
using namespace PlayerCc;

void run_square(char* host, int port, int device_index) {
  PlayerClient robot(host, port);
  Position2dProxy pp(&robot, device_index);

  double move_speed = 0.2;
  double move_turn_init = -5;
  timespec move_sleep_init = {0, 300000000};
  // tomgang i 300 ms
  pp.SetSpeed(move_speed, DTOR(move_turn_init));
  nanosleep(&move_sleep_init, NULL);

  double move_turn_rate = 0.0;
  timespec move_sleep = {5, 100000000};
  double turn_speed = 0.0;
  double turn_turn_rate = 15;
  timespec turn_sleep = {6, 0};

    
  for (int t = 0; t < 4; t++) {

    // GØR DET FØLGENDE FIRE GANGE!
    for (int i = 0; i < 4; i++) {

      // KØR 1 METER FREMAD!

      pp.SetSpeed(move_speed, DTOR(move_turn_rate));
      nanosleep(&move_sleep, NULL);

      // DREJ 90 GRADER!
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
