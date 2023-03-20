#include "pathPlanning.h"

void setDestination(int RID){
    // 存放所有可能路径，pair.first: workStation号码 pair.second: 一个代表距离/时间等等的变量，用来判定
    vector<pair<int, float>> paths = findStation(RID);
    sort(paths.begin(), paths.end(), [](auto &a, auto &b) {return a.second<b.second;});

    for (auto &path : paths){
        if (count(dataTable::destList.begin(), dataTable::destList.end(), path.first) == 0) {       // 如果这个终点没有被其他机器人占用
            dataTable::destList[RID] = path.first;
            break;
        }
    }
    // TODO: 算法待优化，下面是我的还没有写完（没思路了）
    // TODO: 还需要考虑物品售价以及制作
}

vector<pair<int, float>> findStation(int RID) { //寻找可能的目的地
    robot &r = dataTable::robots[RID];
    vector<pair<int, float>> stations;
    if (r.item == 0){
        for(auto &s : dataTable::workStations)
            if (s.proState == 1 || s.timeRemain != -1)                      // 如果机器人空闲，寻找可以生产商品的工作台
                stations.emplace_back(s.id, calculate(RID, s.id));
    }
    else{
        for(auto &s : dataTable::workStations)
            if (identify(r.item, s.type) && (s.matState[r.item] == 0 || s.timeRemain != -1))        // 如果机器人载物，寻找需要原料的工作台
                stations.emplace_back(s.id, calculate(RID, s.id));
    }
    // TODO: 算法优化
    return stations;
}
bool identify(int iType, int sType){

    if (iType == 1){
        if (sType == 4 || sType == 5 || sType == 9)
            return true;
    }
    else if (iType == 2){
        if (sType == 4 || sType == 6 || sType == 9)
            return true;
    }
    else if (iType == 3){
        if (sType == 5 || sType == 6 || sType == 9)
            return true;
    }
    else if (iType == 4 || iType == 5 || iType == 6){
        if (sType == 7 || sType == 9)
            return true;
    }
    else if (iType == 7){
        if (sType == 8 || sType == 9)
            return true;
    }
    return false;
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