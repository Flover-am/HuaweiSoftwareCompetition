#include <iostream>
#include <sstream>
#include <vector>
#include <array>
#include <algorithm>
#include <cmath>

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
array<int,  4> dataTable::destList{-1,-1,-1,-1};
array<bool, 4> dataTable::needRotate{true,true,true,true};

// Logger：日志工具
Logger logger = *new Logger(false);


void initMap();
void readMessage();

// 规划路线的函数
void setDestination();
vector<int> findStation(int);
bool identify(int, int);
float calculate(int, int);

// 输出命令的函数
void navigate(int, int);
void exchange(int, int);

int main() {

    initMap();
    puts("OK");
    fflush(stdout);

    dataTable::frame = 1;
    dataTable::money = START_MONEY;

    for (int i = 0; i < FRAME_NUM; ++i) {
        readMessage();
        printf("%d\n", dataTable::frame++);

        for (int j = 0; j < ROBOT_NUM; ++j)     //  如果有Destination,进行买卖命令
            if (dataTable::destList[j] > 0)
                if (dataTable::destList[j] == dataTable::robots[j].stationID)
                    exchange(j, dataTable::destList[j]);
        for (int j = 0; j < ROBOT_NUM; ++j) {
            if (dataTable::destList[j] < 0) {   //  如果没有Destination,规划下一步动作
                setDestination();
                break;
            }
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
                dataTable::destList[index] = i[j].first;
                dataTable::needRotate[index++] = true;
                break;
            }
        }
    }
    // TODO: 算法待优化，下面是我的还没有写完（没思路了）
    // TODO: 还需要考虑物品售价以及制作
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
vector<int> findStation(int RID) { //寻找可能的目的地
    robot &r = dataTable::robots[RID];
    vector<int> stations;
    if (r.item == 0){
        for(auto &s : dataTable::workStations)
            if (s.proState == 1 || s.timeRemain != -1)                      // 如果工作台生产商品
                stations.emplace_back(s.id);
    }
    else{
        for(auto &s : dataTable::workStations)
            if (identify(s.type, r.item) && s.matState[r.item] == 0)        // 如果工作台需要原料
                stations.emplace_back(s.id);
    }
    // TODO: 算法优化
    return stations;
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
    else if (sType == 7){
        if (iType == 4 || iType == 5 || iType == 6)
            return true;
    }
    else if (sType == 8){
        if (iType == 7)
        return true;
    }
    return true;
}
float calculate(int RID, int SID){
    robot &r = dataTable::robots[RID];
    workStation &s = dataTable::workStations[SID];
    float result;
    // TODO:目前采用简单计算距离的方法，算法待优化，可以计算行进时间与目标工作台工作剩余时间

    float distanceX = abs(r.positionX-s.positionX);
    float distanceY = abs(r.positionY-s.positionY);
    result = distanceX*distanceX+distanceY*distanceY;
    return result;
}

void navigate(int RID, int SID){
    float rx = dataTable::robots[RID].positionX;
    float ry = dataTable::robots[RID].positionY;
    float sx = dataTable::workStations[SID].positionX;
    float sy = dataTable::workStations[SID].positionY;
    robot r = dataTable::robots[RID];

    float distance = sqrt((sy-ry)*(sy-ry)+(sx-rx)*(sx-rx));
    float angle = atan((sy-ry)/(sx-rx));
    if (sx < rx && sy < ry) angle -= PAI;
    if (sx < rx && sy > ry) angle += PAI;
    float alpha = r.direction-angle;
    if (alpha > PAI)        alpha -= 2*PAI;

    if (abs(alpha) < 0.005) //如果偏移角足够小，则不再旋转
        dataTable::needRotate[RID] = false;

    float omega = 0, v = V_MAX; //默认全速直线前进（已经计算过角度回正时如此前进不会错过目的地，但有待验证）
    if (dataTable::needRotate[RID]){
        if (alpha > 0)  omega = OMEGA_MAX >  FRAMES_PER_S*alpha ? FRAMES_PER_S*alpha :  OMEGA_MAX;
        else            omega = OMEGA_MAX > -FRAMES_PER_S*alpha ? FRAMES_PER_S*alpha : -OMEGA_MAX;

        if (abs(alpha) <= PAI/2){
            float cosAlpha = cos(alpha), cosDelta = cos(alpha+omega*TIME_PER_FRAME);
            float tmp = (sqrt((1-cosAlpha*cosAlpha)/(1-cosDelta*cosDelta)));
            float maxV_ = distance*(cosAlpha-cosDelta*tmp); // 根据方程计算v的最大值
            if (!isnan(maxV_) && v < V_MAX)  v = maxV_;
            //墙壁碰撞避免，角度回正时不需要进行
            float collideR = 0.45;  if (r.item != 0) collideR = 0.53; // 是否需要在此处留出空余量？
            if (r.direction > 0){
                float maxV = (50-collideR-ry)*FRAMES_PER_S/sin(r.direction);
                if (v > maxV) v = maxV;
            }
            else{
                float maxV = (collideR-ry)*FRAMES_PER_S/sin(r.direction);
                if (v > maxV) v = maxV;
            }
            if (r.direction < PAI/2 && r.direction > -PAI/2){
                float maxV = (50-collideR-rx)*FRAMES_PER_S/cos(r.direction);
                if (v > maxV) v = maxV;
            }
            else{
                float maxV = (collideR-rx)*FRAMES_PER_S/cos(r.direction);
                if (v > maxV) v = maxV;
            }
        }
    }
    printf("rotate %d %f\n", RID, omega);
    printf("forward %d %f\n", RID, v);
    //TODO:没有精确计算一步运动所需的时间帧数，如可计算可用于路线规划中；另外目前每帧都需要输出两条运动指令，也许可以优化
    //TODO:倒行情况
}
void exchange(int RID, int SID){
    robot &r = dataTable::robots[RID];
    workStation &s = dataTable::workStations[SID];

    bool success = false;           // 由于可能需要等待，需要判定是否操作成功
    if (r.item != 0 && s.matState[r.item] == 1){
        printf("sell %d\n", RID);   // 如果可卖，则卖出物品
        success = true;
    }
    if (r.item == 0 && s.proState == 1){
        printf("buy %d\n", RID);    // 如果可买，则购进原料
        success = true;
    }
    if (success){
        dataTable::needRotate[RID] = true;
        dataTable::destList[RID] = -1;
    }
}
