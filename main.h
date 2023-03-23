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
#include "navigate.h"
#include "exchange.h"

using namespace std;

void initMap();
void readMessage();
void refresh();
void outputNowPath();
void outputPathTree();

void outputPathTree(){
    int i = 0;
    for (auto &pathTree : data::pathTrees){
        logger.writeInfo("RID: " + to_string(i)+ ":");
        for (auto &steps : pathTree){
            string s;
            for (auto &step : steps){
                s += to_string(step.SID) + " ";
            }
            logger.writeInfo(s);
        }
        i++;
    }
}
void outputNowPath(){
    int p = 0;
    for (auto &pathIndex : data::optedPaths){
        string s;
        int x = 0;
        for (auto &stepIndex : pathIndex){
            auto steps = data::pathTrees[p][x];
            s += "SID: " + to_string(steps[stepIndex].SID) + "\t";
            if (x == 2)
                s += to_string(steps[stepIndex].valueSum) + " " + to_string(stepIndex);
            ++x;
        }
        logger.writeInfo(s);
        ++p;
    }
}

int data::frame;
int data::money;
vector<Robot> data::robots;
vector<Station> data::stations;
vector<vector<float>> data::values;
vector<vector<float>> data::times;
array<array<vector<Step>, STEP_DEPTH>, ROBOT_NUM> data::pathTrees;
array<array<int, STEP_DEPTH>, ROBOT_NUM>  data::optedPaths;

array<vector<int>, 8> data::receiveStationIDs;
Logger logger;

#endif //CODECRAFTSDK_MAIN_H
