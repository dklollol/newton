#include <ctime>
#include <libplayerc++/playerc++.h>     // Robot interaction
#include "cam.h"
#include <math.h>
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

int run(char* host, int port, int device_index) {
  
  return EXIT_SUCCESS;
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
    return run(host, port, device_index);
  } catch (PlayerCc::PlayerError e) {
    std::cerr << e << std::endl;
    return EXIT_FAILURE;
  }
}
