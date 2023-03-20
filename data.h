#ifndef CODECRAFTSDK_DATA_H
#define CODECRAFTSDK_DATA_H

#include <vector>
#include "robot.h"
#include "workStation.h"

#define PAI 3.14159f
#define TILE_NUM 100
#define TILE_SIZE 0.5
#define ROBOT_NUM 4
#define FRAME_NUM 9000
#define START_MONEY 200000
#define FRAMES_PER_S 50
#define TIME_PER_FRAME 0.02f
#define VALID_DISTANCE 0.4f

#define PLAN_DEPTH 2

using namespace std;

class data {
public:
    static int frame;
    static int money;
    static vector<robot> robots;
    static vector<workStation> workStations;
    static pair<int, int> destList[ROBOT_NUM][PLAN_DEPTH];    //各个机器人的目的地, 以及它们即将进行的操作
};

#endif //CODECRAFTSDK_DATA_H
