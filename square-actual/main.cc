#include <iostream>
#include <stdio.h>
#include <libplayerc++/playerc++.h>
using namespace PlayerCc;

const char* HOST = "localhost";
const int PORT = 6665;
const int DEVICE_INDEX = 0;

int main(int argc, char *argv[]) {
  try {
    PlayerClient robot(HOST, PORT);
    Position2dProxy pp(&robot, DEVICE_INDEX);

    for (int i = 0; i < 4; i++) {
      // KØR FREMAD!
      double move_speed = 0.2;
      timespec move_sleep = {5, 0};
      double move_turn_rate = 0.0;

      pp.SetSpeed(move_speed, DTOR(move_turn_rate));
      nanosleep(&move_sleep, NULL);

      // DREJ!
      double turn_speed = 0.0;
      timespec turn_sleep = {6, 0};
      double turn_turn_rate = 15.0;

      pp.SetSpeed(turn_speed, DTOR(turn_turn_rate));
      nanosleep(&turn_sleep, NULL);
    }
    
    // STOP!
    timespec stop_sleep = {1, 0};
    pp.SetSpeed(0, 0);
    nanosleep(&stop_sleep, NULL);

  } catch (PlayerCc::PlayerError e) {
    std::cerr << e << std::endl;
    return -1;
  }
}
