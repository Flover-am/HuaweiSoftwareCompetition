#include <iostream>
#include <vector>
#include <array>
#include <sstream>
#include <algorithm>

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
array<int, 4>  dataTable::destList{-1,-1,-1,-1};

// Logger：日志工具
Logger logger = *new Logger(false);


void initMap();
void readMessage();
void setDestination();
bool identify();
float calculate(int, int);
vector<int> findStation(int);

int main() {

    initMap();
    puts("OK");
    fflush(stdout);

    dataTable::frame = 1;
    dataTable::money = START_MONEY;

    for (int i = 0; i < FRAME_NUM; ++i) {
        readMessage();
        std::printf("%d\n", dataTable::frame++);
        for(int j = 0; j < ROBOT_NUM; ++j){
            if (dataTable::destList[j] < 0){     //  如果没有Destination
                setDestination();
                break;
            }
        }
        //TODO: 让机器人执行对应destination的寻路指令，以及到达后的操作指令。注意目前算法下不能保证每次调用setDestination函数能让每个Robot的Destination均可用

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
                    logger.write_error("Robot more than 4.");
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

void setDestination(){
    // 存放所有可能路径，pair.first: workStation号码 pair.second: 一个代表距离/时间等等的变量，用来判定
    array<vector<pair<int, float>>, ROBOT_NUM> paths;
    for(int i = 0; i < ROBOT_NUM; ++i){
        if (dataTable::destList[i] < 0){     //  如果没有Destination
            vector<int> destinations = findStation(i);
            for (int &newD : destinations){
                float value = calculate(i, newD);
                pair<int, float> path(newD, value);
                paths[i].emplace_back(path);
            }
        }
    }

    int index = 0;
    for (auto &i : paths){
        sort(i.begin(), i.end(), [](auto &a, auto &b) {return a.second<b.second;});               // 将每组可能的解按升序排列
        for (int j = 0; j < paths.size(); ++j){
            if (count(dataTable::destList.begin(), dataTable::destList.end(), i[j].first) == 0){  // 如果这个终点没有被其他机器人占用
                dataTable::destList[index++] = i[j].first;
                break;
            }
        }
    }
    // 在目前算法下，方法执行完成后依然有可能有的机器人没有被分配任务，注意
    // TODO: 算法待优化，下面是我的还没有写完（没思路了）
    /*vector<array<pair<int, float>, 4>> solutions; // 存放所有可能的解法
    int x = min_element(paths.begin(), paths.end(),
                [](auto &a, auto &b)
                {return b.size()==0||(a.size()!=0&&a.size()<b.size());}
            )-paths.begin();                      // 寻找可能的路径最少的机器人编号
    for (int i = 0; i < paths[x].size(); ++i){    // 建立每种可能的解法
        array<pair<int, float>, 4> path;
        path[0]= paths[x][i];
        solutions.push_back(path);
    }*/
}
bool identify(int sType, int iType){
    if (sType == 1 || sType == 2 || sType == 3)
        return false;
    else if (sType == 4){
        if (iType == 1 || iType == 2)
            return true;
    }
    else if (sType == 5){
        if (iType == 1 || iType == 3)
            return true;
    }
    else if (sType == 6){
        if (iType == 2 || iType == 3)
            return true;
    }
    else{
        if (iType == 4 || iType == 5 || iType == 6)
            return true;
    }
    return false;
}
float calculate(int RID, int SID){
    workStation &s = dataTable::workStations[SID];
    robot &r = dataTable::robots[RID];
    float result;
    // TODO:目前采用简单计算距离的方法，算法待优化，可以计算行进时间与目标工作台工作剩余时间

    float distanceX = abs(r.positionX-s.positionX);
    float distanceY = abs(r.positionY-s.positionY);
    result = distanceX*distanceX+distanceY*distanceY;
    return result;
}
vector<int> findStation(int RID) {
    robot &r = dataTable::robots[RID];
    vector<int> stations;
    if (r.item == 0){
        for(auto &s : dataTable::workStations)
            if (s.proState == 1 || s.timeRemain != -1)                      // 如果有商品
                stations.emplace_back(s.id);
    }
    else{
        for(auto &s : dataTable::workStations)
            if (identify(s.type, r.item) && s.matState[r.item] == 0)        // 如果没有原料
                stations.emplace_back(s.id);
    }
    // TODO: 算法优化
    return stations;
}
