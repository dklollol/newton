#ifndef USING_STAGE

#include <ctime>

void sleep(double seconds) {
  struct timespec spec;
  spec.tv_sec = (time_t) seconds;
  spec.tv_nsec = (long) ((seconds - (double) spec.tv_sec) * 10e8);
  nanosleep(&spec, NULL);
}

#else

#include <libplayerc++/playerc++.h>
using namespace PlayerCc;

double get_time() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (double) tv.tv_sec + (double) tv.tv_usec / 10e6;
}

void sleep_for(double seconds, PlayerClient* robot) {
  double start = get_time();
  double end = start + seconds;
  while (get_time() < end) {
    robot->Read();
  }
}

#define sleep(seconds) sleep_for(seconds, &robot)

#endif
