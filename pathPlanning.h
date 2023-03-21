#ifndef CODECRAFTSDK_PATHPLANNING_H
#define CODECRAFTSDK_PATHPLANNING_H

#include <deque>
#include <algorithm>

#include "data.h"
#include "Logger.h"

// 规划路线的函数
void setDestination(int);
float calculate(int, int, bool);
deque<pair<int, int>> findStation(int);

extern Logger logger;

#endif //CODECRAFTSDK_PATHPLANNING_H
