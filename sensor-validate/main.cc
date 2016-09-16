#include <libplayerc++/playerc++.h>
#include <iostream>
#include <args-debugexperiments.h>
#include <scorpion.h>
#include <time.h>
#include <curses.h>
using namespace PlayerCc;

int detections[13] = {};

bool checkSensor(int index, float threshold, IrProxy ir) {

  if (ir.GetRange(index) < threshold && detections[index] < 10) {
    detections[index] += 1;
  } else {
    detections[index] = 0;      
  }

  return (detections[index] > 9);
}
void run_square(char* host, int port, int device_index) {
  PlayerClient robot(host, port);
  Position2dProxy pp(&robot, device_index);
  IrProxy ir(&robot, gIndex);

  double move_speed = 0.1;
  double turn_speed = 10;

  bool collision = false;

  while (true) {
    
    //Check for near collision
    for (int i = 0; i < 9; i++) {
      if (checkSensor(i, 0.5, ir)) {
        collision = true;
      }
    }

    if (collision) {      
      pp.SetSpeed(0, DTOR(turn_speed));
    } else {
      pp.SetSpeed(move_speed, 0);      
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
    run_square(host, port, device_index);
    return EXIT_SUCCESS;
  } catch (PlayerCc::PlayerError e) {
    std::cerr << e << std::endl;
    return EXIT_FAILURE;
  }
}
