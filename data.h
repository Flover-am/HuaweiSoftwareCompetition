#ifndef CODECRAFTSDK_DATA_H
#define CODECRAFTSDK_DATA_H

#include <vector>
#include "robot.h"
#include "workStation.h"

#define PAI 3.1415926535f
#define TILE_NUM 100
#define TILE_SIZE 0.5
#define ROBOT_NUM 4
#define FRAME_NUM 9000
#define START_MONEY 200000
#define FRAMES_PER_S 50
#define TIME_PER_FRAME 0.02f
#define VALID_DISTANCE 0.4f

#define STEP_DEPTH 2

#define TIME_COLL 0.5
using namespace std;

class data {
public:
    static int frame;
    static int money;
    static vector<robot> robots;
    static vector<workStation> workStations;
    static array<array<pair<int, int>, STEP_DEPTH>, ROBOT_NUM> destList;
    // 存放所有路径，int1.workStation号码 int2.对应操作
    static array<vector<int>, 8> receiveStationIDs;
    static array<bool, 4> have_printed;

    // 存放可以接受各种物品作为原料的所有工作台
};

#endif //CODECRAFTSDK_DATA_H
