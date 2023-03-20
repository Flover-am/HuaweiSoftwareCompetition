#ifndef CODECRAFTSDK_DATATABLE_H
#define CODECRAFTSDK_DATATABLE_H

#include <vector>
#include <array>
#include "robot.h"
#include "workStation.h"

#define PAI 3.14159
#define X_POS (j+0.25f)
#define Y_POS (100-(i+0.25f))
#define ROBOT_NUM 4
#define FRAME_NUM 9000
#define START_MONEY 200000
#define FRAMES_PER_S 50
#define TIME_PER_FRAME 0.02f
#define V_MAX 6.0f
#define OMEGA_MAX PAI
#define MAP_SIZE 50
#define VALID_DISTANCE 0.4f

using namespace std;

class dataTable {
public:
    int a;
    static array<bool, ROBOT_NUM> needRotate;           //是否需要在行进中旋转
    static array<int, ROBOT_NUM> destList;  //各个机器人的目的地
    static int frame;
    static int money;
    static vector<robot> robots;
    static vector<workStation> workStations;


    dataTable(int);
};


#endif //CODECRAFTSDK_DATATABLE_H
