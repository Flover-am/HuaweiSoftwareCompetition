#ifndef CODECRAFTSDK_PATHPLANNING_H
#define CODECRAFTSDK_PATHPLANNING_H

#include <tuple>
#include <vector>
#include <algorithm>

#include "data.h"

// 规划路线的函数
void setDestination(int);
bool identify(int, int);
float calculate(int, int);
vector<tuple<int, int, float>> findStation(int);

#endif //CODECRAFTSDK_PATHPLANNING_H
