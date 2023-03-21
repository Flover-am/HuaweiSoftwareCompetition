#ifndef CODECRAFTSDK_DATA_H
#define CODECRAFTSDK_DATA_H

#include <vector>
#include "robot.h"
#include "station.h"

#define PAI 3.14159f
#define TILE_NUM 100
#define TILE_SIZE 0.5
#define ROBOT_NUM 4
#define FRAME_NUM 9000
#define START_MONEY 200000
#define FRAMES_PER_S 50
#define TIME_PER_FRAME 0.02f
#define VALID_DISTANCE 0.4f

#define STEP_DEPTH 2

using namespace std;

struct Step {
    int SID{-1};
    int OID{-1};
    int timeStamp{-1};
    float value{0};
    int lastIndex{-1};
    vector<int> nextIndex{-1};
};

class data {
public:
    static int frame;
    static int money;
    static vector<robot> robots;
    static vector<station> stations;
    // 存放所有路径树
    static array<array<vector<Step>, STEP_DEPTH>, ROBOT_NUM> pathTrees;
    // 存放选择的路径号
    static array<int, ROBOT_NUM> pathNums;
    // 存放可以接受各种物品作为原料的所有工作台
    static array<vector<int>, 8> receiveStationIDs;

};

#endif //CODECRAFTSDK_DATA_H
