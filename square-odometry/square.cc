#include <iostream>
#include <libplayerc++/playerc++.h>
using namespace PlayerCc;
 
void set_pull_mode(PlayerClient &robot) {
  robot.SetDataMode(PLAYER_DATAMODE_PULL);
  robot.SetReplaceRule(true, PLAYER_MSGTYPE_DATA, -1, -1);
}

void sleep(double seconds) {
  struct timespec spec;
  spec.tv_sec = (time_t) seconds;
  spec.tv_nsec = (long) ((seconds - (double) spec.tv_sec) * 10e8);
  nanosleep(&spec, NULL);
}

void run_square(char* host, int port, int device_index) {
  PlayerClient robot(host, port);
  set_pull_mode(robot);
  Position2dProxy pp(&robot, device_index);

  pp.SetOdometry(0, 0, 0);
  
  printf("START; x: %lf, y: %lf\n", pp.GetXPos(), pp.GetYPos());
  pp.SetSpeed(0.2, 0.0);
  sleep(5.0);
  printf("MIDDLE; x: %lf, y: %lf\n", pp.GetXPos(), pp.GetYPos());
  pp.SetSpeed(0.0, DTOR(15.0));
  sleep(6.0);
  pp.SetSpeed(0.2, 0.0);
  sleep(5.0);
  printf("END; x: %lf, y: %lf\n", pp.GetXPos(), pp.GetYPos());
  pp.SetSpeed(0.0, 0.0);
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
