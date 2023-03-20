#ifndef CODECRAFTSDK_PATHPLANNING_H
#define CODECRAFTSDK_PATHPLANNING_H

#include <vector>
#include <algorithm>

#include "dataTable.h"

// 规划路线的函数
void setDestination();
vector<pair<int, float>> findStation(int);
bool identify(int, int);
float calculate(int, int);

#endif //CODECRAFTSDK_PATHPLANNING_H
