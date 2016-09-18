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


#define N_SENSORS 13
int detections[N_SENSORS];

bool checkSensor(int index, double threshold, IrProxy* ir) {
  if (ir->GetRange(index) < threshold && detections[index] < 10) {
    detections[index] += 1;
  } else {
    detections[index] = 0;
  }

  return (detections[index] > 9);
}

void run(char* hostname, int port, int device_index) {
  PlayerClient robot(hostname, port);
  set_pull_mode(&robot);

  Position2dProxy pp(&robot, device_index);
  IrProxy ir(&robot, device_index);

  for (int i = 0; i < N_SENSORS; i++) {
    detections[i] = 0;
  }

  double move_speed = 0.2;
  double turn_speed = 10;

  bool collision = false;

  while (true) {
    // Check for near collision.
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
