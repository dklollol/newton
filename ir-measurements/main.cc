#include <libplayerc++/playerc++.h>
#include "sleep.hpp"
using namespace PlayerCc;

void wait() {
  getc(stdin);
}

void run(char* hostname, int port, int device_index) {
  PlayerClient robot(hostname, port);
  IrProxy ir(&robot, device_index);

  FILE* f = fopen("measurements.txt", "w");

  double dist;
  for (int i = 0; i <= 40; i += 10) {
    printf("Ready to measure distance at %d centimeters?\n", i);
    wait();
    fprintf(f, "%d cm\n", i);
    for (int j = 0; j < 20; j++) {
      robot.Read();
      //sleep(1);
      dist = ir.GetRange(2);
      printf("Read: %lf\n", dist);
      fprintf(f, "%lf ", dist);
      sleep(0.5);
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
