#include <iostream>
#include <libplayerc++/playerc++.h>
using namespace PlayerCc;

void run_square(char* host, int port, int device_index) {
  PlayerClient robot(host, port);
  Position2dProxy pp(&robot, device_index);

  double move_speed = 0.2;
  double turn_rate = 20.0;
  timespec move_sleep = {18, 200000000};
  timespec move_straight = {0, 100000000};

  pp.SetSpeed(move_speed, 0);
  nanosleep(&move_straight, NULL);

  while (true) {
    //move_speed = 0.2;
    turn_rate = 20.0;
    //move_sleep = {12, 000000000};

    pp.SetSpeed(move_speed, DTOR(turn_rate));
    nanosleep(&move_sleep, NULL);

    //pp.SetSpeed(move_speed, 0);
    //nanosleep(&move_straight, NULL);
    //nanosleep(&move_straight, NULL);

    //move_speed = 0.2;
    turn_rate = -20.0;
    //move_sleep = {12, 000000000};

    pp.SetSpeed(move_speed, DTOR(turn_rate));
    nanosleep(&move_sleep, NULL);
  }
  
  // // STOP!
  // timespec stop_sleep = {1, 0};
  // pp.SetSpeed(0, 0);
  // nanosleep(&stop_sleep, NULL);
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
