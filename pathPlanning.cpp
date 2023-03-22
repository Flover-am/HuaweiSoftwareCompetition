#include "pathPlanning.h"
void selectPath(int RID){

    auto &pathTree = data::pathTrees[RID];
    vector<float> weights;
    vector<bool> valids;
    // 刷新权值与时间戳
    for (int robotNum = 0; robotNum < ROBOT_NUM; ++robotNum){
        for (auto &step : data::pathTrees[robotNum][0]){
            int SID = step.SID, OID = step.OID;
            pair<float, float> value = calculateValue(RID, SID, OID, true);
            step.frame = step.frameSum = (int)value.first;
            step.value = step.valueSum = value.second;
        }
        for (int depth = 1; depth < STEP_DEPTH; ++depth){
            for (auto &step : data::pathTrees[robotNum][depth]){
                auto &lastIndex = step.lastIndex;
                auto &lastStep = data::pathTrees[robotNum][depth-1][lastIndex];
                step.frameSum = step.frame+lastStep.frameSum;
                step.valueSum = step.value+lastStep.valueSum;
            }
        }
    }
    auto width = pathTree[STEP_DEPTH-1].size();
    // 冲突检测: 初始化
    for (auto &steps : pathTree)
        for (auto &step : steps)
            step.valid = true;
    // 加载
    for (auto &steps : pathTree)
        for (auto &step : steps) {
            // 如果只卖，不会发生冲突
            if (step.OID == ONLY_SELL){
                logger.writeInfo("ONLY SELL", false);
                continue;
            }
            // 如果端口不可用
            auto &station = data::stations[step.SID];
            if (station.portAvailableTime[step.OID] == -1){
                logger.writeInfo("INVALID", false);
                step.valid = false;
            }
            // TODO: bug here如果发生冲突
            auto &asks = station.portRecvAskTime[step.OID];
            auto size = asks.size();

            if (size > 1){ //有冲突可能
                logger.writeInfo("CONFLICT", false);
                for (auto &ask : asks)
                    logger.writeInfo(to_string(ask.RID), false);

                int thisIndex = 0; // 找到本step的对应请求号
                for (int i = 0; i < asks.size(); ++i)
                    if (RID == asks[i].RID && step.frameSum == asks[i].timeStamp){
                        thisIndex = i;
                        break;
                    }
                int conflictRID = -1;
                for (auto &ask : asks)
                    if (ask.RID != RID && ask.RID != conflictRID){
                        conflictRID = ask.RID;
                        if (ask.value >= step.value){ // TODO:VALUE
                            asks.erase(asks.begin()+thisIndex);
                            step.valid = false;
                            break;
                        }
                    }
            }
        }
    // 写入
    for (int i = 0; i < width; ++i){
        int index = i;
        bool valid = true;
        for (int depth = STEP_DEPTH-1; depth >= 0; --depth){
            auto &step = pathTree[depth][index];
            valid = valid && step.valid;
            index = step.lastIndex;

        }
        valids.emplace_back(valid);
    }
    // 将权值加和
    for (int i = 0; i < width; ++i){
        if (!valids[i]){
            weights.emplace_back(0);
            continue;
        }
        double weight = 0;
        int index = i;
        for (int depth = STEP_DEPTH-1; depth >= 0; --depth){
            weight += pathTree[depth][index].value;
            index = pathTree[depth][index].lastIndex;
        }
        weights.emplace_back(weight);
    }
    // 选择
    int pathIndex = 0;
    for (int i = 0; i < width; ++i)
        if (weights[i] > weights[pathIndex])
            pathIndex = i;
    for (int depth = STEP_DEPTH-1; depth >= 0; --depth){
        data::optedPaths[RID][depth] = pathIndex;
        pathIndex = pathTree[depth][pathIndex].lastIndex;
    }
}
void setPathTree(int RID) {
    robot &r = data::robots[RID];
    auto &pathTree = data::pathTrees[RID];

    int depthNeeded = 1;                // 计算需要规划的步骤深度
    while (depthNeeded < STEP_DEPTH){
        if (pathTree[STEP_DEPTH-depthNeeded-1].empty())
            depthNeeded++;
        else
            break;
    }
    // 构建路径树
    for (int depth = STEP_DEPTH-depthNeeded; depth < STEP_DEPTH; ++depth) {
        // 如果需要从头构建
        if (depth == 0){
            auto steps = findStation(RID, r.item, 0, true);
            double maxValue = 0;
            for (auto &step : steps)
                maxValue = step.value > maxValue ? step.value : maxValue;

            for (auto &step : steps){
                if (step.value*FACTOR < maxValue)
                    continue;
                pathTree[depth].emplace_back(step);
            }
        }
        else{
            vector<Step> tmpSteps;
            // 集中所有此步得到的steps
            int lastIndex = 0;
            for (const auto &lastStep : pathTree[depth-1]){
                int item = 0;
                if (lastStep.OID == ONLY_BUY)
                    item = data::stations[lastStep.SID].type;

                auto steps = findStation(lastStep.SID, item, lastIndex, false);
                for (auto &step : steps)
                    tmpSteps.emplace_back(step);
                lastIndex++;
            }
            // 去除不合标准的step
            double maxValue = 0;
            for (auto &step : tmpSteps)
                maxValue = step.value > maxValue ? step.value : maxValue;
            auto size = tmpSteps.size();
            for (int i = 0, index = 0; i < size; ++i){
                if (tmpSteps[index].value*FACTOR < maxValue)
                    tmpSteps.erase(tmpSteps.begin()+index);
                else
                    ++index;
            }
            // 写入
            pathTree[depth] = tmpSteps;
            for (int index = 0; index < pathTree[depth].size(); ++index){
                lastIndex = pathTree[depth][index].lastIndex;
                pathTree[depth-1][lastIndex].nextIndex.emplace_back(index);
            }
        }
    }
    // 可能有不完整的路径，但予以保存

    // TODO: 目前卖出同时不会买入
    // TODO: 时间不足时应该不再买入
}
vector<Step> findStation(int RID, int IID, int lastIndex, bool firstStep){
    vector<Step> stations;

    if (IID == 0){
        for(auto &s : data::stations)
            // 如果机器人空闲，寻找可以生产商品的工作台
            if (s.type <= 7){
                if (s.portAvailableTime[0] != -1){ //TODO: ?
                    pair<double, double> value = calculateValue(RID, s.id, ONLY_BUY, firstStep);
                    vector<int> newNext;
                    Step newStep{true, s.id, ONLY_BUY,
                                 (int)value.first, (int)value.first,
                                 value.second, value.second,
                                 lastIndex, newNext};
                    stations.emplace_back(newStep);
                }
            }
    }
    else{
        for (int &SID : data::receiveStationIDs[IID]) {
            auto &s = data::stations[SID];
            if (s.type == 8 || s.type == 9){
                pair<double, double> value = calculateValue(RID, s.id, ONLY_SELL, firstStep);
                vector<int> newNext;
                Step newStep{true, s.id, ONLY_SELL,
                             (int)value.first,(int)value.first,
                             value.second, value.second,
                             lastIndex, newNext};
                stations.emplace_back(newStep);
            }

            else{
                pair<double, double> value = calculateValue(RID, s.id, IID, firstStep);
                vector<int> newNext;
                Step newStep{true, s.id, IID,
                             (int)value.first, (int)value.first,
                             value.second, value.second,
                             lastIndex, newNext};
                stations.emplace_back(newStep);
            }
        }
    }
    // TODO: 算法优化
    return stations;
}
pair<double, double> calculateValue(int RID, int SID, int OID, bool firstStep){
    station &s = data::stations[SID];
    pair<double, double> value;
    value.first = calculateTime(RID, SID, OID, firstStep);
    value.second = 0;
    if (OID == ONLY_BUY){
        if (s.type == 1)
            value.second = WORTH_1;
        else if (s.type == 2)
            value.second = WORTH_2;
        else if (s.type == 3)
            value.second = WORTH_3;
        else if (s.type == 4)
            value.second = WORTH_4;
        else if (s.type == 5)
            value.second = WORTH_5;
        else if (s.type == 6)
            value.second = WORTH_6;
        else if (s.type == 7)
            value.second = WORTH_7;
    }
    else{
        int item;
        if (firstStep)  item = data::robots[RID].item;
        else            item = data::stations[RID].type;
        if (item == 1)
            value.second = WORTH_1;
        else if (item == 2)
            value.second = WORTH_2;
        else if (item == 3)
            value.second = WORTH_3;
        else if (item == 4)
            value.second = WORTH_4;
        else if (item == 5)
            value.second = WORTH_5;
        else if (item == 6)
            value.second = WORTH_6;
        else if (item == 7)
            value.second = WORTH_7;
        if (item != 7 && OID == ONLY_SELL)
            value.second /= 2;
    }
    value.second = value.second/value.first;
    return value;
}




