#include <libplayerc++/playerc++.h>
using namespace PlayerCc;

void wait() {
  getc(stdin);
}

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

  IrProxy ir(&robot, device_index);

  FILE* f = fopen("measurements.txt", "w");

  double dist;
  for (int i = 10; i <= 60; i += 10) {
    printf("Ready to measure distance at %d centimeters?\n", i);
    wait();
    fprintf(f, "%d cm\n", i);
    for (int j = 0; j < 20; j++) {
      robot.Read();
      dist = ir.GetRange(2);
      fprintf(f, "%lf ", dist);
      sleep(0.01);
    }
    fprintf(f, "\n");
  }

  fclose(f);
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
