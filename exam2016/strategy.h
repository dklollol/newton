#ifndef EXAM2016_STRATEGY_H
#define EXAM2016_STRATEGY_H

#include <libplayerc++/playerc++.h>
#include <map>
#include "robot.h"
#include "misc.h"
#include "camera.h"
#include "random_numbers.h"
#include "particles.h"


enum driving_state_t {searching_sqaure, searching_random, approach, align,
                      goto_landmark, finished};
//static bool visitedlandmarks= []
static map<object::type, bool> visited_landmarks = {
  {object::landmark1, false},
  {object::landmark2, false},
  {object::landmark3, false},
  {object::landmark4, false},
};
static map<object::type, bool> seen_landmarks = {
  {object::landmark1, false},
  {object::landmark2, false},
  {object::landmark3, false},
  {object::landmark4, false},
};


static std::map<driving_state_t, string> stateMap = {
  {goto_landmark, "Going to landmark"},
  {searching_random, "searching_random"},
  {searching_sqaure, "searching_sqaure"},
  {approach, "approach"},
  {driving_state_t::align, "align"},
  {finished, "finished"}
};

void execute_strategy(Position2dProxy &pp, pos_t &pos, particle &p,
                      driving_state_t &driving_state, object::type landmark,
                      double measured_distance, double measured_angle);

#endif
