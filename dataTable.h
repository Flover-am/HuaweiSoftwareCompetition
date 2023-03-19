#ifndef CODECRAFTSDK_DATATABLE_H
#define CODECRAFTSDK_DATATABLE_H

#include <vector>
#include <array>
#include "robot.h"
#include "workStation.h"

#define X_POS (j+0.25f)
#define Y_POS (100-(i+0.25f))
#define ROBOT_NUM 4
#define FRAME_NUM 9000
#define START_MONEY 200000

using namespace std;

class dataTable {
public:
    int a;
    static array<int, ROBOT_NUM> destList; //各个机器人的目的地
    static int frame;
    static int money;
    static vector<robot> robots;
    static vector<workStation> workStations;

    dataTable(int);
};


#endif //CODECRAFTSDK_DATATABLE_H
