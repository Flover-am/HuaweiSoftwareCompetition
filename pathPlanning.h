#ifndef CODECRAFTSDK_PATHPLANNING_H
#define CODECRAFTSDK_PATHPLANNING_H

#include <deque>
#include <algorithm>

#include "data.h"
#include "Logger.h"
#include "navigate.h"
#include "material.h"

// 规划路线的函数
void selectPath(int);
void setPathTree(int);
vector<Step> findStation(int, int, int, bool);
pair<float, float> calculateValue(int, int, int, bool);

#endif //CODECRAFTSDK_PATHPLANNING_H
