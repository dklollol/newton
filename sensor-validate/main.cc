#include <libplayerc++/playerc++.h>
#include <ctime>
using namespace PlayerCc;

bool checkSensor(int index, float threshold, IrProxy &ir) {
  return (ir.GetRange(index) < threshold);
}
void run_square(char* host, int port, int device_index) {
  PlayerClient robot(host, port);
  Position2dProxy pp(&robot, device_index);
  IrProxy ir(&robot, gIndex);

  double move_speed = 0.2;
  double turn_speed = 30;

  bool collision = false;

  while (true) {
    robot.Read();
    collision = false;
    
    //Check for near collision
    for (int i = 0; i < 9; i++) {
      if (checkSensor(i, 0.5, &ir)) {
        collision = true;
      }
    }

    if (collision) {
      pp.SetSpeed(0, DTOR(turn_speed));
    } else {
      pp.SetSpeed(move_speed, 0);
    }
    sleep(0.01);
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
