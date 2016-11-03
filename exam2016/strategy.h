#ifndef EXAM2016_STRATEGY_H
#define EXAM2016_STRATEGY_H

#include <libplayerc++/playerc++.h>
#include <map>
#include "robot.h"
#include "misc.h"
#include "camera.h"


enum driving_state_t {search_turn, searching_sqaure, searching_random, approach, align};
//static bool seenlandmarks= []
static map<object::type, bool> visited_landmarks = {
  {object::landmark1, false},
  {object::landmark2, false},
  {object::landmark3, false},
  {object::landmark4, false},
};
static std::map<driving_state_t, string> stateMap = {
  {search_turn, "search_turn"},
  {searching_random, "searching_random"},
  {searching_sqaure, "searching_sqaure"},
  {approach, "approach"},
  {driving_state_t::align, "align"}
};

void execute_strategy(Position2dProxy &pp, pos_t &pos,
                      driving_state_t &driving_state, object::type landmark,
                      double measured_distance, double measured_angle);

#endif
