#ifndef CODECRAFTSDK_PATHPLANNING_H
#define CODECRAFTSDK_PATHPLANNING_H

#include <deque>
#include <algorithm>

#include "data.h"

// 规划路线的函数
void setDestination(int);
bool identify(int, int);
float calculate(int, int, bool);
deque<pair<int, int>> findStation(int);
//vector<pair<pair<int, int>[PLAN_DEPTH], float> findStation(int);

#endif //CODECRAFTSDK_PATHPLANNING_H
