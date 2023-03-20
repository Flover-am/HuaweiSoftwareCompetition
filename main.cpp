#include <iostream>
#include <sstream>
#include <vector>
#include <array>

#include "robot.h"
#include "workStation.h"
#include "data.h"
#include "Logger.h"

#include "pathPlanning.h"
#include "navigate.h"
#include "exchange.h"

using namespace std;
// 储存数据的工具：包括frame，money，robot，workstation
int data::frame;
int data::money;
vector<robot> data::robots;
vector<workStation> data::workStations;
array<array<pair<int, int>, STEP_DEPTH>, ROBOT_NUM> data::destList;

// Logger：日志工具
Logger logger = *new Logger(false);

void initMap();
void readMessage();

int main() {

    initMap();
    puts("OK");
    fflush(stdout);

    data::frame = 1;
    data::money = START_MONEY;
    for (int i = 0; i < ROBOT_NUM; ++i)
        for (int j = 0; j < STEP_DEPTH; ++j) {
            pair<int, int> element(-1, -1);
            data::destList[i][j] = element;
        }

    for (int i = 0; i < FRAME_NUM; ++i) {
        readMessage();
        printf("%d\n", data::frame++);

        for (int j = 0; j < ROBOT_NUM; ++j)
            if (data::destList[j][STEP_DEPTH-1].first < 0)      //  如果规划不完全，准备规划路线
                setDestination(j);
        for (int j = 0; j < ROBOT_NUM; ++j)
            if (data::destList[j][0].first >= 0){               //  如果有下一步规划，准备行动
                if (data::destList[j][0].first == data::robots[j].stationID)   // 如果已经抵达目标,进行买卖命令
                    exchange(j, data::destList[j][0]);
                if (data::destList[j][0].first != data::robots[j].stationID)   // 如果有Destination且尚未抵达,进行移动命令
                    navigate(j, data::destList[j][0].first);
            }

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

            float posX = (j+0.5)*TILE_SIZE, posY = (TILE_NUM-i-0.5)*TILE_SIZE;
            if (symbol == 'A'){
                data::robots.emplace_back(robot++, posX, posY);
                if (robot > ROBOT_NUM)
                    logger.writeError("Robot more than 4.", true);
            }
            else if (isdigit(symbol))
                data::workStations.emplace_back(symbol - 48, workStation++, posX, posY);
        }
    }
    cin >> ws;
}
void readMessage() {
    string line;
    getline(cin, line);
    stringstream ss(line);
    ss >> data::frame >> data::money;
    getline(cin, line);

    int skipInt = 0;    float skipFloat = 0;
    for (auto &s : data::workStations) {
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
    for (auto &r : data::robots) {
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

