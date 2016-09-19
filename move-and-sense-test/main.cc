#include <libplayerc++/playerc++.h>
#include <ctime>
using namespace PlayerCc;

void sleep(double seconds) {
  struct timespec spec;
  spec.tv_sec = (time_t) seconds;
  spec.tv_nsec = (long) ((seconds - (double) spec.tv_sec) * 10e8);
  nanosleep(&spec, NULL);
}

void set_pull_mode(PlayerClient *robot) {
  robot->SetDataMode(PLAYER_DATAMODE_PULL);
  robot->SetReplaceRule(true, PLAYER_MSGTYPE_DATA, -1, -1);
}

void run(char* hostname, int port, int device_index) {
  PlayerClient robot(hostname, port);
  set_pull_mode(&robot);

  Position2dProxy pp(&robot, device_index);
  IrProxy ir(&robot, device_index);

  double speed = 0.2;
  double turn = 0.0;

  pp.SetSpeed(speed, DTOR(turn));

  while (true) {
    sleep(0.8);
    robot.Read();
    printf("Number of IR range sensors: %i\n", ir.GetCount());
    for (uint i = 0; i < ir.GetCount(); i++) {
      printf("IR sensor %i: %f\n", i, ir.GetRange(i));
    }
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
