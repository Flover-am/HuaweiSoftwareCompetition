#ifndef CODECRAFTSDK_PATHPLANNING_H
#define CODECRAFTSDK_PATHPLANNING_H

#include <deque>
#include <algorithm>

#include "data.h"
#include "Logger.h"
#include "navigate.h"
#include "material.h"

// 规划路线的函数
void setDestination(int);
deque<pair<int, int>> findStation(int);
float calculateValue(int, pair<int, int>, bool);

#endif //CODECRAFTSDK_PATHPLANNING_H
