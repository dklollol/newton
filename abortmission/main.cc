#include <iostream>
#include <libplayerc++/playerc++.h>
using namespace PlayerCc;

void run_abortmission(char* host, int port, int device_index) {
  PlayerClient robot(host, port);
  Position2dProxy pp(&robot, device_index);
  IrProxy ir(&robot, 0);

  // We have to read and wait a little to get subscriptions..
  robot.Read();
  sleep(5);
  robot.Read();

  double move_speed = 0.2;
  timespec move_sleep_init = {1, 0};

  while (true) {
    printf("%f,", ir.GetRange(2));
    nanosleep(&move_sleep_init, NULL);
  }
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
    run_abortmission(host, port, device_index);
    return EXIT_SUCCESS;
  } catch (PlayerCc::PlayerError e) {
    std::cerr << e << std::endl;
    return EXIT_FAILURE;
  }
}