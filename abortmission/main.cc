#include <libplayerc++/playerc++.h>
#include "sleep.hpp"
using namespace PlayerCc;

void run(char* hostname, int port, int device_index) {
  PlayerClient robot(hostname, port);
  Position2dProxy pp(&robot, device_index);
  IrProxy ir(&robot, device_index);

  double move_speed = 0.2;

  while (true) {
    robot.Read();
    if (ir.GetRange(2) < 0.5) {
      if (ir.GetRange(4) - ir.GetRange(3) > 0) {
        pp.SetSpeed(0, DTOR(30)); //NW
      } else {
        pp.SetSpeed(0, DTOR(-30)); //NE
      }
    } else {
      pp.SetSpeed(move_speed, 0); //N
    }
    sleep(0.01); // 0.02 seconds == 20 milliseconds
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
    run(host, port, device_index);
    return EXIT_SUCCESS;
  } catch (PlayerCc::PlayerError e) {
    std::cerr << e << std::endl;
    return EXIT_FAILURE;
  }
}
