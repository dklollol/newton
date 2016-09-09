#include <iostream>
#include <libplayerc++/playerc++.h>
using namespace PlayerCc;

void run_square(char* host, int port, int device_index) {
  PlayerClient robot(host, port);
  Position2dProxy pp(&robot, device_index);

  double move_speed = 0.2;
  timespec move_sleep = {0, 500000000};
  double turn_rate = 0.0;

  double magic = 45;
  int x = 0;
  while (true) {

    if (x >= 360) {
      turn_rate = sin(DTOR(x)) * magic;
    }
    else {
      turn_rate = -sin(DTOR(x)) * magic;
    }
    printf("x: %d\n", x);
    printf("turn rate: %f\n", turn_rate);
    
    pp.SetSpeed(move_speed, DTOR(turn_rate));
    nanosleep(&move_sleep, NULL);

    x = x + 10;
    if (x > 720) {
      x = 0;
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
