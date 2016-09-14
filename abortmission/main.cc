#include <libplayerc++/playerc++.h>
#include <iostream>
#include <args-debugexperiments.h>
#include <scorpion.h>
#include <time.h>
#include <curses.h>
using namespace PlayerCc;

void run_abortmission(char* host, int port, int device_index) {
  PlayerClient robot(host, port);
  Position2dProxy pp(&robot, device_index);
  IrProxy ir(&robot, gIndex);

  double move_speed = 0.2;
  double turn_speed = 0;

  int count = ir.GetCount();
  printf("%d\n", count);

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
    //nanosleep(&move_sleep_init, NULL);
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
