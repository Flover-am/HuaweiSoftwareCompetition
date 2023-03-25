#include <iostream>
#include <sstream>
#include <vector>
#include <array>

#include "robot.h"
#include "workStation.h"
#include "data.h"
#include "Logger.h"
#include "avoidCollision.h"
#include "pathPlanning.h"
#include "conflictRecall.h"
#include "navigate.h"
#include "exchange.h"

using namespace std;
// 储存数据的工具：包括frame，money，robot，workstation
int data::frame;
int data::money;
vector<robot> data::robots;
vector<workStation> data::workStations;
array<array<pair<int, int>, STEP_DEPTH>, ROBOT_NUM> data::destList;
array<vector<int>, 8> data::receiveStationIDs;
array<bool, 4> data::have_printed{false};

// Logger：日志工具
Logger logger = Logger(false);

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

    for (int frame = 0; frame < FRAME_NUM; ++frame) {
        readMessage();
        printf("%d\n", data::frame++);
        for (int robotNum = 0; robotNum < ROBOT_NUM; ++robotNum) {
            // 如果需要购买，检测之后是否存在新的出售冲突

            if (ONLY_BUY == data::destList[robotNum][STEP_DEPTH - 1].second) {
                auto SID = data::destList[robotNum][STEP_DEPTH - 1].first;
                detectConflict(robotNum, data::workStations[SID].type);
            }
        }


        for (int robotNum = 0; robotNum < ROBOT_NUM; ++robotNum) {
            if (data::destList[robotNum][STEP_DEPTH - 1].first < 0) {
                setDestination(robotNum);
            }     //  如果规划不完全，准备规划路线

        }


        for (int robotNum = 0; robotNum < ROBOT_NUM; ++robotNum) {
            if (data::destList[robotNum][0].first >= 0) {               //  如果有下一步规划，准备行动
                if (data::destList[robotNum][0].first == data::robots[robotNum].stationID) {
                    exchange(robotNum, data::destList[robotNum][0]);
                    navigate(robotNum, data::destList[robotNum][0].first, true);
                }   // 如果已经抵达目标,进行买卖命令

                if (data::destList[robotNum][0].first !=
                    data::robots[robotNum].stationID)   // 如果有Destination且尚未抵达,进行移动命令
                    navigate(robotNum, data::destList[robotNum][0].first);
            } else {
                logger.writeError(to_string(data::frame) + "  No Des!");
            }
        }

        data::have_printed = {false};
        out:
        puts("OK");
        fflush(stdout);
    }
//
//    ifstream prams("prams.in");
//    float time = 0;
//    float dis_deta = 0;
//    prams >> time >> dis_deta;
//    prams.close();
//    if (dis_deta > 1.5) {
//        dis_deta = 0.2;
//        time += 0.1;
//    } else {
//        dis_deta += 0.1;
//    }
//    ofstream prams_re("prams.in", ios::out);
//    prams_re << time << " " << dis_deta << endl;
//    prams_re.close();
//    ofstream score("score.out", ios::app);
//    score << " " << time << " , " << dis_deta << " , " << data::money << "," << endl;

    return 0;
}

void initMap() {
    int robot = 0, workStation = 0;
    string line;

    for (int i = 0; getline(cin, line); i++) {
        if (line == "OK")
            break;

        for (int j = 0; j < 100; ++j) {
            char symbol = line[j];
            if (symbol == '.')
                continue;

            float posX = (j + 0.5) * TILE_SIZE, posY = (TILE_NUM - i - 0.5) * TILE_SIZE;
            if (symbol == 'A') {
                data::robots.emplace_back(robot++, posX, posY);
                if (robot > ROBOT_NUM)
                    logger.writeError("Robot more than 4.", true);
            } else if (isdigit(symbol))
                data::workStations.emplace_back(symbol - '0', workStation++, posX, posY);
        }
    }
    for (const auto &station: data::workStations) {
        if (station.type == 4) {
            data::receiveStationIDs[1].emplace_back(station.id);
            data::receiveStationIDs[2].emplace_back(station.id);
        } else if (station.type == 5) {
            data::receiveStationIDs[1].emplace_back(station.id);
            data::receiveStationIDs[3].emplace_back(station.id);
        } else if (station.type == 6) {
            data::receiveStationIDs[2].emplace_back(station.id);
            data::receiveStationIDs[3].emplace_back(station.id);
        } else if (station.type == 7) {
            data::receiveStationIDs[4].emplace_back(station.id);
            data::receiveStationIDs[5].emplace_back(station.id);
            data::receiveStationIDs[6].emplace_back(station.id);
        } else if (station.type == 8) {
            data::receiveStationIDs[7].emplace_back(station.id);
        } else if (station.type == 9) {
            data::receiveStationIDs[1].emplace_back(station.id);
            data::receiveStationIDs[2].emplace_back(station.id);
            data::receiveStationIDs[3].emplace_back(station.id);
            data::receiveStationIDs[4].emplace_back(station.id);
            data::receiveStationIDs[5].emplace_back(station.id);
            data::receiveStationIDs[6].emplace_back(station.id);
            data::receiveStationIDs[7].emplace_back(station.id);
        }
    }
}

void readMessage() {
    string line;
    getline(cin, line);
    stringstream ss(line);
    ss >> data::frame >> data::money;
    getline(cin, line);

    int skipInt = 0;
    float skipFloat = 0;
    for (auto &s: data::workStations) {
        int number;
        getline(cin, line);
        ss.clear();
        ss.str(line);
        ss >> skipInt >> skipFloat >> skipFloat;
        ss >> s.timeRemain >> number >> s.proState;
        for (auto &state: s.matState) {
            state = number % 2;
            number /= 2;
        }
    }
    for (auto &r: data::robots) {
        getline(cin, line);
        ss.clear();
        ss.str(line);
        ss >> r.stationID >> r.item >> r.tValue >> r.hValue;
        ss >> r.angleV >> r.lineVX >> r.lineVY >> r.direction >> r.positionX >> r.positionY;
        if (r.item != 0) {
            r.r = R_BIG;
            r.m = M_BIG;
            r.a = A_BIG;
            r.alpha = ALPHA_BIG;
        } else {
            r.r = R_SMALL;
            r.m = M_SMALL;
            r.a = A_SMALL;
            r.alpha = ALPHA_SMALL;
        }
    }
    getline(cin, line);
}

