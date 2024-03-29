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
                path.second += calculateValue(RID, path.first[step], true);
            else
                path.second += calculateValue(path.first[step-1].first, path.first[step], false);
        }
    // 按权值排序，最终确定一条路径
    sort(paths.begin(), paths.end(), [](auto &a, auto &b) {return a.second<b.second;});

    for (auto &path : paths){
        for (int stepNum = STEP_DEPTH-depthNeeded; stepNum < STEP_DEPTH; ++stepNum) {
            auto step = path.first[stepNum];
            if (step.second == ONLY_SELL) // 最终卖出，不会冲突
                goto in;

           /* bool canSell = false;
            if (step.second == ONLY_BUY){
                int item = step.first;
                for(auto &s : data::workStations)
                    if (identify(item, s.type) && (s.matState[item] == 0 *//*|| ss.timeRemain != -1*//*)*//*且原料台均已满*//*){
                        canSell = true;// 场上有地方卖
                        break;
                    }
            }
            if (!canSell)
                goto out;*/

            for (auto &fixedPath: data::destList) for (auto &fixedStep: fixedPath) {// 对于每一个确定好的路径
                if (step == fixedStep)    // 如果步骤冲突，则检查下一条
                    goto out;
            }
        }
        in : data::destList[RID] = path.first;   //均无冲突，结束
        return;
        out: continue;
    }
    // TODO: 目前卖出同时不会买入
    // TODO: 时间不足时不再买入
}
deque<pair<int, int>> findStation(int item){
    deque<pair<int, int>> stations;
    if (item == 0){
        for(auto &s : data::workStations)
            // 如果机器人空闲，寻找可以生产商品的工作台
            if (s.proState == 1 || s.timeRemain != -1){
                item = s.type;
                bool canSell = false;
                // 寻找对应的出售地点
                for (int &SID : data::receiveStationIDs[item]){
                    auto &sellS = data::workStations[SID];

                    for (auto &fixedPath: data::destList) for (auto &fixedStep: fixedPath)      // 判定潜在的卖出冲突
                        if (fixedStep.first == sellS.id)                                        // 若去同一个工作台
                            if (fixedStep.second == item && fixedStep.second != ONLY_SELL)      // 若有冲突
                                goto out;
                    // 如果场上有可卖的工作台
                    if (sellS.matState[item] == 0 /*|| (ss.timeRemain != -1 且 原料台均已满)*/) {
                        canSell = true;
                        break;
                    }
                    out: continue;
                }
                if (canSell)
                    stations.emplace_back(s.id, ONLY_BUY);
            }
    }
    else{
        for (int &SID : data::receiveStationIDs[item]) {
            auto &s = data::workStations[SID];
            if (s.matState[item] == 0 /*|| (ss.timeRemain != -1 且 原料台均已满)*/) {
                if (s.type == 8 || s.type == 9)
                    stations.emplace_back(s.id, ONLY_SELL);
                else
                    stations.emplace_back(s.id, item);
                //TODO: r.item与robot中定义的宏相对应，需用于其它机器人调用查看行动是否冲突
            }
        }
    }
    // TODO: 算法优化
    return stations;
}
float calculateValue(int RID, pair<int, int> step, bool firstStep){
    workStation &s = data::workStations[step.first];
    float time = calculateTime(RID, step, firstStep);

    float worth = 0;
    if (step.second == ONLY_BUY){
        if (s.type == 1)
            worth = WORTH_1;
        else if (s.type == 2)
            worth = WORTH_2;
        else if (s.type == 3)
            worth = WORTH_3;
        else if (s.type == 4)
            worth = WORTH_4;
        else if (s.type == 5)
            worth = WORTH_5;
        else if (s.type == 6)
            worth = WORTH_6;
        else if (s.type == 7)
            worth = WORTH_7;
    }
    else{
        int item;
        if (firstStep)  item = data::robots[RID].item;
        else            item = data::workStations[RID].type;
        if (item == 1)
            worth = WORTH_1;
        else if (item == 2)
            worth = WORTH_2;
        else if (item == 3)
            worth = WORTH_3;
        else if (item == 4)
            worth = WORTH_4;
        else if (item == 5)
            worth = WORTH_5;
        else if (item == 6)
            worth = WORTH_6;
        else if (item == 7)
            worth = WORTH_7;
        if (item != 7 && step.second == ONLY_SELL)
            worth /= 2;
    }
    float value = time/worth;
    return value;
}


