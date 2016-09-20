#include <libplayerc++/playerc++.h>
#include <ctime>
using namespace PlayerCc;

void sleep(double seconds) {
  struct timespec spec;
  spec.tv_sec = (time_t) seconds;
  spec.tv_nsec = (long) ((seconds - (double) spec.tv_sec) * 10e8);
  nanosleep(&spec, NULL);
}

void set_pull_mode(PlayerClient &robot) {
  robot.SetDataMode(PLAYER_DATAMODE_PULL);
  robot.SetReplaceRule(true, PLAYER_MSGTYPE_DATA, -1, -1);
}

float ir_correction(float range) {
  return range * 0.93569 - 0.103488;
}

bool check_sensor(int index, float threshold, IrProxy &ir) {	
  return (ir_correction(ir.GetRange(index)) < threshold);
}

void run(char* host, int port, int device_index) {
  PlayerClient robot(host, port);
  set_pull_mode(robot);

  Position2dProxy pp(&robot, device_index);
  IrProxy ir(&robot, device_index);

  double move_speed = 0.2;
  double turn_speed = 30;

  bool collision = false;
  while (true) {
    robot.Read();
    collision = false;

    // Check for near collision.
    for (int i = 0; i < 9; i++) {
      if (check_sensor(i, 0.5, ir)) {
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
