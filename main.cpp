#include <iostream>
#include <sstream>
#include <vector>
#include <array>

#include "robot.h"
#include "workStation.h"
#include "dataTable.h"
#include "Logger.h"

#include "pathPlanning.h"
#include "navigate.h"
#include "exchange.h"

using namespace std;

// 储存数据的工具：包括frame，money，robot，workstation
int dataTable::frame;
int dataTable::money;
vector<robot> dataTable::robots;
vector<workStation> dataTable::workStations;
array<int, 4> dataTable::destList{-1,-1,-1,-1};

// Logger：日志工具
Logger logger = *new Logger(false);

void initMap();
void readMessage();

int main() {

    initMap();
    puts("OK");
    fflush(stdout);

    dataTable::frame = 1;
    dataTable::money = START_MONEY;

    for (int i = 0; i < FRAME_NUM; ++i) {
        readMessage();
        printf("%d\n", dataTable::frame++);

        for (int j = 0; j < ROBOT_NUM; ++j)     //  如果有Destination且已经抵达,进行买卖命令
            if (dataTable::destList[j] >= 0)
                if (dataTable::destList[j] == dataTable::robots[j].stationID)
                    exchange(j, dataTable::destList[j]);
        for (int j = 0; j < ROBOT_NUM; ++j)
            if (dataTable::destList[j] < 0) {   //  如果没有Destination,规划下一步动作
                setDestination();
                break;
            }
        for (int j = 0; j < ROBOT_NUM; ++j)     //  如果有Destination且尚未抵达,进行移动命令
            if (dataTable::robots[j].stationID != dataTable::destList[j])
                navigate(j, dataTable::destList[j]);

        puts("OK");
        fflush(stdout);
    }
    return 0;
}

void initMap() {
    int robot = 0, workStation = 0;
    string line;

    for (int i = 0; getline(cin, line); i++) {
        if (line == "OK")
            return;

        for (int j = 0; j < 100; ++j) {
            char symbol = line[j];
            if (symbol == '.')
                continue;

            if (symbol == 'A'){
                dataTable::robots.emplace_back(robot++, X_POS, Y_POS);
                if (robot > ROBOT_NUM)
                    logger.writeError("Robot more than 4.", true);
            }
            else if (isdigit(symbol))
                dataTable::workStations.emplace_back(symbol-48, workStation++, X_POS, Y_POS);
        }
    }
    cin >> ws;
}
void readMessage() {
    string line;
    getline(cin, line);
    stringstream ss(line);
    ss >> dataTable::frame >> dataTable::money;
    getline(cin, line);

    int skipInt = 0;    float skipFloat = 0;
    for (auto &s : dataTable::workStations) {
        int number;
        getline(cin, line);
        ss.clear();
        ss.str(line);
        ss >> skipInt >> skipFloat >> skipFloat;
        ss >> s.timeRemain >> number >> s.proState;
        for (auto &state : s.matState){
            state = number%2;
            number /= 2;
        }
    }
    for (auto &r : dataTable::robots) {
        getline(cin, line);
        ss.clear();
        ss.str(line);
        ss >> r.stationID >> r.item >> r.tValue >> r.hValue;
        ss >> r.angleV >> r.lineVX >> r.lineVY >> r.direction >> r.positionX >> r.positionY;
        if(r.item != 0){
            r.r = R_BIG;
            r.m = M_BIG;
            r.a = A_BIG;
            r.alpha = ALPHA_BIG;
        }
        else{
            r.r = R_SMALL;
            r.m = M_SMALL;
            r.a = A_SMALL;
            r.alpha = ALPHA_SMALL;
        }
    }
    getline(cin, line);
}

