#ifndef EXAM2016_TIMING_H
#define EXAM2016_TIMING_H

#include <sys/time.h>

struct timing_t {
  struct timeval time_start;
  struct timeval time_end;
  struct timeval time_diff;
  unsigned long usecs;
};

void timing_start(timing_t* timing);

void timing_end(timing_t* t);

// FIXME: The macros below should really be handled with generated dependency
// files in the Makefile to make dependencies clear.

#define TIMER_START() \
  { \
  timing_t timer; \
  double timer_dur_ms; \
  timing_start(&timer);

#define TIMER_END(description) \
  timing_end(&timer); \
  timer_dur_ms = (double) timer.usecs / 1000.0; \
  if (timer_dur_ms >= 10.0) { \
    printf("[TIMER] " description ": %lf.3 ms\n", timer_dur_ms); \
  } \
  }

#endif
