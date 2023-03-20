#include "pathPlanning.h"

void setDestination(int RID) {
    int depthNeeded = 1;  // 需要规划的步骤数目
    while (depthNeeded < STEP_DEPTH){
        if (data::destList[RID][STEP_DEPTH-depthNeeded-1].first == -1)
            depthNeeded++;
        else
            break;
    }

    robot &r = data::robots[RID];
    // 初始化，int1: 工作站编号 int2: 操作名称 float: 权值
    pair<array<pair<int, int>, STEP_DEPTH>, float> tmpPath(data::destList[RID], 0);
    deque<pair<array<pair<int, int>, STEP_DEPTH>, float>> paths{tmpPath};

    for (int stepNum = STEP_DEPTH-depthNeeded; stepNum < STEP_DEPTH; ++stepNum) {
        int size = (int)(paths.size());
        for (int count = 0; count < size; ++count) {
            // 确定当前步骤，机器人携带的物品号
            int item = 0;
            if (stepNum == 0)
                item = r.item;
            else if (paths[0].first[stepNum-1].second == ONLY_BUY)
                item = data::workStations[paths[0].first[stepNum-1].first].type;

            // 寻找新的步骤
            auto steps = findStation(item);
            auto incompletePath = paths[0];                 // 上一段循环得到的不完整的路径
            paths.pop_front();
            for (auto &step : steps){
                auto completePath = incompletePath;
                completePath.first[stepNum] = step;         // 在后面添加新的步骤，补全路径
                paths.emplace_back(completePath);
            }
        }
    }
    // 计算权值
    for (auto &path : paths)
        for (int step = STEP_DEPTH-depthNeeded; step < STEP_DEPTH; ++step){
            if (step == 0)
                path.second += calculate(RID, path.first[step].first, true);
            else
                path.second += calculate(path.first[step-1].first, path.first[step].first, false);
        }
    // 按权值排序，最终确定一条路径
    for (auto &path : paths){
        sort(paths.begin(), paths.end(), [](auto &a, auto &b) {return a.second<b.second;});
        if (count(data::destList.begin(), data::destList.end(), path.first) == 0) {       // 如果这个终点没有被其他机器人占用
            data::destList[RID] = path.first;
            break;
        }
    }
    // TODO: 还需要考虑物品售价以及制作时长
    // TODO: 目前卖出同时不会买入
}

bool identify(int iType, int sType) {
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
float calculate(int RID, int SID, bool firstStep) {
    workStation &s = data::workStations[SID];
    float distanceX, distanceY;
    if (firstStep){
        auto &r = data::robots[RID];
        distanceX = abs(r.positionX-s.positionX);
        distanceY = abs(r.positionY-s.positionY);
    }
    else{
        auto &ss = data::workStations[RID];
        distanceX = abs(ss.positionX-s.positionX);
        distanceY = abs(ss.positionY-s.positionY);
    }
    float result;
    // TODO:目前采用简单计算距离的方法，算法待优化，可以计算行进时间与目标工作台工作剩余时间
    result = distanceX*distanceX+distanceY*distanceY;
    return result;
}
deque<pair<int, int>> findStation(int item){
    deque<pair<int, int>> stations;
    if (item == 0){
        for(auto &s : data::workStations)
            // 如果机器人空闲，寻找可以生产商品的工作台
            if (s.proState == 1 || s.timeRemain != -1)
                stations.emplace_back(s.id, ONLY_BUY);
    }
    else{
        for(auto &s : data::workStations)
            // 如果机器人载物，寻找需要原料的工作台
            if (identify(item, s.type) && (s.matState[item] == 0 || s.timeRemain != -1)) {
                if (s.type == 8 || s.type == 9)
                    stations.emplace_back(s.id, ONLY_SELL);
                else
                    stations.emplace_back(s.id, item);
                //TODO: r.item与robot中定义的宏相对应，需用于其它机器人调用查看行动是否冲突
            }

    }
    // TODO: 算法优化
    return stations;
}