#include <iostream>
#include <vector>
#include <sstream>

#include "robot.h"
#include "workStation.h"
#include "Logger.h"
#include "dataTable.h"

using namespace std;

// 储存数据的工具：包括frame，money，robot，workstation
int dataTable::frame;
int dataTable::money;
vector<robot> dataTable::robots;
vector<workStation> dataTable::workStations;

// Logger：日志工具
Logger logger = *new Logger(false);


// 初始化地图
void init_map();

// 读取每一帧的数据
bool readMessage();

int main() {


    // 与判题器握手
    init_map();
    puts("OK");
    fflush(stdout);

    dataTable::frame = 1;
    dataTable::money = 200000;

    while (readMessage()) {

        printf("%d\n", dataTable::frame++);
        float lineSpeed = 6;
        float angleSpeed = 1;
        for (int robotId = 0; robotId < 4; robotId++) {
            printf("forward %f %f\n", robotId, lineSpeed);
            printf("rotate %f %f\n", robotId, angleSpeed);
        }
        printf("OK\n");
        fflush(stdout);
    }
    return 0;
}

void init_map() {
    char symbol;
    int robot_now = 0;
    int worStation_now = 0;
    string line;
    int i = 0;
    while (getline(cin, line)) {

        if (line[0] == 'O' && line[1] == 'K') {
            return;
        }
        for (int j = 0; j < 100; ++j) {
            symbol = line[j];
            if (symbol == '.') {
                continue;
            }
            if (symbol == 'A') {
                dataTable::robots.emplace_back(robot_now++, j + 0.25f, 100 - (i + 0.25f));
                if (robot_now > 4) {
                    logger.write_error("robot less than 4 ???");
                }
            } else if (isdigit(symbol) != 0) {
                dataTable::workStations.emplace_back(symbol - 48, worStation_now++, j + 0.25f, 100 - (i + 0.25f));
            }
        }
        i++;
    }
    cin >> ws;
}

bool readMessage() {
    string line;
    getline(cin, line);
    stringstream ss(line);
    // 读取第一行
    ss >> dataTable::frame >> dataTable::money >> ws;
    // 屏蔽第二行
    getline(cin, line);
    int skip = 0;
    for (int i = 0; i < dataTable::workStations.size(); ++i) {
        int a = 1;
        //TODO: 每帧数据更新
    }
}
