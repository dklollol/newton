#ifndef EX5_TIMING_H
#define EX5_TIMING_H

#include <sys/time.h>

typedef struct {
  struct timeval time_start;
  struct timeval time_end;
  struct timeval time_diff;
  unsigned long usecs;
} timing_t;

void timing_start(timing_t* timing);

void timing_end(timing_t* t);

#define TIMER_START() \
  { \
  timing_t timer; \
  timing_start(&timer);

#define TIMER_END(description) \
  timing_end(&timer); \
  printf("[TIMING] " description ": %zd\n", timer.usecs); \
  }

#endif
