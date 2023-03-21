#ifndef CODECRAFTSDK_MAIN_H
#define CODECRAFTSDK_MAIN_H

#include <iostream>
#include <sstream>
#include <vector>
#include <array>

#include "robot.h"
#include "station.h"
#include "data.h"
#include "Logger.h"

#include "pathPlanning.h"
#include "conflictRecall.h"
#include "navigate.h"
#include "exchange.h"

using namespace std;

void initMap();
void readMessage();

int data::frame;
int data::money;
vector<robot> data::robots;
vector<station> data::stations;
array<array<vector<Step>, STEP_DEPTH>, ROBOT_NUM> data::pathTrees;
array<int, ROBOT_NUM> data::pathNums;

array<vector<int>, 8> data::receiveStationIDs;
Logger logger = *new Logger(false);

#endif //CODECRAFTSDK_MAIN_H
