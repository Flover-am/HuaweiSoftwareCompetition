#ifndef CODECRAFTSDK_DATA_H
#define CODECRAFTSDK_DATA_H

#include <vector>
#include "robot.h"
#include "station.h"

#define PAI 3.14159
#define TILE_NUM 100
#define TILE_SIZE 0.5
#define ROBOT_NUM 4
#define FRAME_NUM 9000
#define START_MONEY 200000
#define FRAMES_PER_S 50
#define TIME_PER_FRAME 0.02
#define VALID_DISTANCE 0.4

#define STEP_DEPTH 3

using namespace std;

struct Step {
    int SID{-1};
    int OID{-1};
    int frame{-1};
    int frameSum{-1};
    double value{0};
    double valueSum{0};
    int lastIndex{-1};
    vector<int> nextIndex;
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
    static array<array<int, STEP_DEPTH>, ROBOT_NUM> optedPaths;
    // 存放可以接受各种物品作为原料的所有工作台
    static array<vector<int>, 8> receiveStationIDs;

    /*static string optedPathsToString(){
        string s;
    }*/

};

#endif //CODECRAFTSDK_DATA_H
