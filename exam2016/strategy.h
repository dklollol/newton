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
                      goto_landmark, finished, test_search, test_goto};
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
  {searching_random, "Searching random"},
  {searching_sqaure, "Searching sqaure"},
  {approach, "Approach"},
  {driving_state_t::align, "Align"},
  {finished, "Finished"},
  {test_search, "Test search"},
  {test_goto, "Test going to landmark"}
};

void execute_strategy(PlayerClient &robot,
                      Position2dProxy &pp, IrProxy &ir, pos_t &pos, particle &p,
                      driving_state_t &driving_state, object::type landmark,
                      double measured_distance, double measured_angle);

#endif
