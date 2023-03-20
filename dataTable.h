#ifndef CODECRAFTSDK_DATATABLE_H
#define CODECRAFTSDK_DATATABLE_H

#include <vector>
#include <array>
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

using namespace std;

class dataTable {
public:
    int a;
    static array<int, ROBOT_NUM> destList;              //各个机器人的目的地
    static int frame;
    static int money;
    static vector<robot> robots;
    static vector<workStation> workStations;

    dataTable(int);
};

#endif //CODECRAFTSDK_DATATABLE_H
