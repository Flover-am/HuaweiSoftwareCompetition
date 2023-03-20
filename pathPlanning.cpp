#include "pathPlanning.h"

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
        sort(i.begin(), i.end(), [](auto &a, auto &b) {return a.second<b.second;});               // 将每组可能的解按值升序排列
        for (auto &path : i){
            if (count(dataTable::destList.begin(), dataTable::destList.end(), path.first) == 0){  // 如果这个终点没有被其他机器人占用
                dataTable::destList[index++] = path.first;
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