#include <iostream>
#include <vector>
#include <sstream>

#include "robot.h"
#include "workStation.h"
#include "Logger.h"
#include "dataTable.h"

#define X_POS (j+0.25f)
#define Y_POS (100-(i+0.25f))

using namespace std;

// 储存数据的工具：包括frame，money，robot，workstation
int dataTable::frame;
int dataTable::money;
vector<robot> dataTable::robots;
vector<workStation> dataTable::workStations;

// Logger：日志工具
Logger logger = *new Logger(false);


void init_map();
void readMessage();

int main() {

    init_map();
    puts("OK");
    fflush(stdout);

    dataTable::frame = 1;
    dataTable::money = 200000;

    for (int i = 0; i < 9000; ++i) {
        readMessage();
        std::printf("%d\n", dataTable::frame++);
        //TODO: Commands.
//        需要实现的方法：
//        1. 机器人从当前位置寻路至指定工作台。
//        2. 计算距离每个机器人最近的可用的工作台。
//        3. 在机器人到达指定工作台后，进行操作。

        puts("OK");
        fflush(stdout);
    }
    return 0;
}

void init_map() {
    int robot = 0, workStation = 0;
    string line;

    for (int i = 0; getline(cin, line); i++) {
        if (line == "OK")
            return;

        for (int j = 0; j < 100; ++j) {
            char symbol = line[j];

            if (symbol == 'A')
                dataTable::robots.emplace_back(robot++, X_POS, Y_POS);
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

    int skip = 0;
    for (auto &s : dataTable::workStations) {
        int number;
        getline(cin, line);
        ss.clear(); ss.str(line);
        ss >> skip >> skip >> skip;
        ss >> s.timeRemain >> number >> s.proState;
        for (auto &state : s.matState){
            state = number%2;
            number /= 2;
        }
    }
    for (auto &r : dataTable::robots) {
        getline(cin, line);
        ss.clear(); ss.str(line);
        ss >> r.stationID >> r.item >> r.tValue >> r.hValue;
        ss >> r.angleV >> r.lineVX >> r.lineVY >> r.direction >> r.positionX >> r.positionY;
    }
    getline(cin, line);
}
