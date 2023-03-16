#include <iostream>
#include <vector>

#include "robot.h"
#include "workStation.h"
#include "Logger.h"

using namespace std;

bool readUntilOK() {
    char line[1024];
    while (fgets(line, sizeof line, stdin)) {
        if (line[0] == 'O' && line[1] == 'K') {
            return true;
        }
        //do something
    }
    return false;
}

void init_map(vector<robot> &robots, vector<workStation> &workStations);

Logger logger = *new Logger();


int main() {
    // 与判题器握手
    readUntilOK();
    puts("OK");
    fflush(stdout);

    //初始化数据；
    vector<robot> robots(4);
    vector<workStation> workStations(0);

    init_map(robots, workStations);

    int frameID;

    while (scanf("%d", &frameID) != EOF) {
        readUntilOK();
        printf("%d\n", frameID);
        int lineSpeed = 2;
        double angleSpeed = 2;
        for (int robotId = 0; robotId < 4; robotId++) {
            printf("forward %d %d\n", robotId, lineSpeed);
            printf("rotate %d %f\n", robotId, angleSpeed);
        }
        printf("OK\n", frameID);
        fflush(stdout);
    }
    return 0;
}

void init_map(vector<robot> &robots, vector<workStation> &workStations) {
    char symbol;
    int robot_now = 0;
    for (int i = 0; i < 100; ++i) {
        for (int j = 0; j < 100; ++j) {
            cin >> symbol;
//            logger.write_log(to_string(symbol));
            if (symbol == '.') {
                continue;
            }
            if (symbol == 'A') {
                robots[robot_now] = robot(j + 0.25f, 100 - (i + 0.25f));
                robot_now++;
                if (robot_now > 4) {
                    logger.write_log("1");
                }
            } else if (isdigit(symbol) != 0) {
                workStations.emplace_back(j + 0.25f, 100 - (i + 0.25f));
            }
        }
    }
    cin >> ws;
}
