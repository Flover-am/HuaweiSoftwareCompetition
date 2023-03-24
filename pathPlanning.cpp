#include "pathPlanning.h"

void selectPath(int RID){
    auto &pathTree = data::pathTrees[RID];
    vector<float> weights;

    // 对于每一条虚路径，检测其是否可用
    for (int depth = 0; depth < STEP_DEPTH; ++depth) {
        auto &steps = pathTree[depth];
        for (auto &step : steps) {
            // 剪枝，减小计算量
            if (depth > 0)
                if (pathTree[depth-1][step.lastIndex].frameSum >= UNAVAILABLE){
                    step.frameSum = UNAVAILABLE;
                    continue;
                }
            int portID = step.OID;
            int validFrame = UNAVAILABLE-1;
            auto &station = data::stations[step.SID];
            // 非引用！避免修改原condition
            auto tmpCondition = station.condition;
            // 如果只卖，不会发生冲突
            if (portID == ONLY_SELL) {
                validFrame = 0;
                goto out;
            }
            // 如果作为原料卖出，检测冲突
            if (portID > ONLY_BUY)
                for (; validFrame >= 0; validFrame--){
                    auto &Frame = tmpCondition[validFrame];
                    // 如果有冲突
                    if (!Frame[portID].empty()){
                        auto askIDs = Frame[portID];
                        auto size = askIDs.size();

                        int askIndex = 0; // 请求在condition.frame.port中的编号
                        for (int j = 0; j < size; ++j, ++askIndex){
                            auto askID = askIDs[askIndex];
                            if (askID == -1)
                                goto out;
                            else{
                                auto &ask = station.asks[askID];
                                // TODO：如果对应的请求就是本虚步骤，或者冲突比较中本步骤胜出
                                if (ask.RID <= RID && ask.timeStamp == step.frameSum || ask.timeStamp > step.frameSum){
                                    for (int frame = ask.timeStamp; frame <= validFrame; ++frame){
                                        auto &asksToErase = tmpCondition[frame][portID];
                                        asksToErase.erase(asksToErase.begin()+askIndex); //在tmpCondition中去除这个请求
                                    }
                                    --askIndex;
                                }
                                else
                                    validFrame++;
                            }
                        }
                    }
                }   // 倒序寻找
            else { // portID == ONLY_BUY
                int prosNum = station.prosNum[UNAVAILABLE-1];
                // 如果最后一刻依然没有产品
                if (prosNum == 0)
                    goto out;
                auto &Frame = tmpCondition[UNAVAILABLE-1];
                auto &askIDs = Frame[portID];

                // 将所有请求加载到asks，找到本步骤的优先级序号
                Ask thisAsk{RID, step.OID, step.frameSum};
                vector<Ask> asks{thisAsk};
                for (auto askID: askIDs){
                    auto &ask = station.asks[askID];
                    if (ask.RID == RID && ask.timeStamp == step.frameSum)
                        continue;
                    asks.emplace_back(ask);
                }
                sort(asks.begin(), asks.end(), [](auto a, auto b){return a.timeStamp < b.timeStamp;});

                int rank = 0;
                for (; rank < asks.size(); ++rank){
                    auto &ask = asks[rank];
                    if (ask.RID == RID && ask.timeStamp == step.frameSum)
                        break;
                }
                // 如果优先级不足，退出
                for (; validFrame >= 0; validFrame--)
                    if (station.prosNum[validFrame] <= rank)
                        goto out;
            } // 倒序寻找
            // 计算frameSum
            out :
            int lastFrameSum = step.frame;
            if (depth > 0){
                int lastIndex = step.lastIndex;
                lastFrameSum += pathTree[depth-1][lastIndex].frameSum;
            }
            step.frameSum = lastFrameSum > validFrame+1?
                            lastFrameSum : validFrame+1;
        }
    }

    auto width = pathTree[STEP_DEPTH-1].size();
    // 计算weight
    for (int i = 0; i < width; ++i) {
        auto &lastStep = pathTree[STEP_DEPTH-1][i];
        if (lastStep.frameSum >= UNAVAILABLE)
            weights.emplace_back(0);
        else
            weights.emplace_back(lastStep.valueSum/lastStep.frameSum);
    }


    // 选择
    int pathIndex = 0;
    for (int i = 0; i < width; ++i){
        if (weights[i] > weights[pathIndex])
            pathIndex = i;
    }
    for (int depth = STEP_DEPTH-1; depth >= 0; --depth){
        data::optedPaths[RID][depth] = pathIndex;
        pathIndex = pathTree[depth][pathIndex].lastIndex;
    }
}

void setPathTree(int RID) {
    Robot &r = data::robots[RID];
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
            for (auto &step : steps)
                pathTree[depth].emplace_back(step);
        }
        else{

            int lastIndex = 0, index = 0;
            for (const auto &lastStep : pathTree[depth-1]){
                int item = 0;
                if (lastStep.OID == ONLY_BUY)
                    item = data::stations[lastStep.SID].type;

                auto steps = findStation(lastStep.SID, item, lastIndex, false);
                for (auto &step : steps){
                    pathTree[depth].emplace_back(step);
                    pathTree[depth-1][lastIndex].nextIndex.emplace_back(index++);
                }
                lastIndex++;
            }
        }
    }
    // 计算valueSum
    for (int depth = STEP_DEPTH-depthNeeded; depth < STEP_DEPTH; ++depth) {
        if (depth > 0){
            for (auto &step : pathTree[depth]){
                int lastIndex = step.lastIndex;
                step.valueSum = pathTree[depth-1][lastIndex].valueSum+step.value;
            }
        }
    }
    // TODO: 目前卖出同时不会买入
    // TODO: 时间不足时应该不再买入
}
vector<Step> findStation(int RID, int IID, int lastIndex, bool firstStep) {
    vector<Step> stations;

    vector<int> newNext;
    if (IID == 0){
        for(auto &s : data::stations)
            // 如果机器人空闲，寻找可以生产商品的工作台
            if (s.type <= 7){
                if (s.prosNum[UNAVAILABLE-1] > 0){
                    float time, value;
                    if (!firstStep){
                        time = data::times[RID][s.id];
                        value = data::values[RID][s.id];
                    }
                    else{
                        time = calculateNowTime(RID, s.id);
                        value = calculateValue(s.id, ONLY_BUY);
                    }
                    Step newStep{s.id, ONLY_BUY,
                                 int(time), int(time),
                                 value, value,
                                 lastIndex, newNext};
                    stations.emplace_back(newStep);
                }
            }
    }
    else{

        for (int &SID : data::receiveStationIDs[IID]) {

            auto &s = data::stations[SID];

            if (s.type == 8 || s.type == 9){
                float time, value;
                if (!firstStep){
                    time = data::times[RID][s.id];
                    value = data::values[RID][s.id];
                }
                else{
                    time = calculateNowTime(RID, s.id);
                    value = calculateValue(s.id, ONLY_SELL);
                }
                Step newStep{s.id, ONLY_SELL,
                             int(time), int(time),
                             value, value,
                             lastIndex, newNext};
                stations.emplace_back(newStep);
            }
            else{
                float time, value;
                if (!firstStep){
                    time = data::times[RID][s.id];
                    value = data::values[RID][s.id];
                }
                else{
                    time = calculateNowTime(RID, s.id);
                    value = calculateValue(s.id, IID);
                }
                Step newStep{s.id, IID,
                             int(time), int(time),
                             value, value,
                             lastIndex, newNext};
                stations.emplace_back(newStep);
            }
        }
    }
    return stations;
}
float calculateTime(int SID1, int SID2) {
    auto &s1 = data::stations[SID1];
    auto &s2 = data::stations[SID2];
    float distance;
    float sx1 = s1.positionX, sy1 = s1.positionY;
    float sx2 = s2.positionX, sy2 = s2.positionY;

    distance = sqrt((sy2-sy1)*(sy2-sy1)+(sx2-sx1)*(sx2-sx1));
    float time = 50*(distance/V_MAX);
    // TODO: 在其他位置计算等待时间
    return time;
}
float calculateValue(int SID, int OID) {
    Station &s = data::stations[SID];
    float weight;
    if (OID == ONLY_BUY){
        if (s.type == 1)
            weight = WORTH_1;
        else if (s.type == 2)
            weight = WORTH_2;
        else if (s.type == 3)
            weight = WORTH_3;
        else if (s.type == 4)
            weight = WORTH_4;
        else if (s.type == 5)
            weight = WORTH_5;
        else if (s.type == 6)
            weight = WORTH_6;
        else //if (s.type == 7)
            weight = WORTH_7;

    }
    else if (OID == PROD_SELL_1)
        weight = WORTH_1;
    else if (OID == PROD_SELL_2)
        weight = WORTH_2;
    else if (OID == PROD_SELL_3)
        weight = WORTH_3;
    else if (OID == PROD_SELL_4)
        weight = WORTH_4;
    else if (OID == PROD_SELL_5)
        weight = WORTH_5;
    else if (OID == PROD_SELL_6)
        weight = WORTH_6;
    else{
        if (s.type == 8)
            weight = WORTH_7;
        else
            weight = WORTH_7/2; //TODO: 应该是对应的物品号除以一个值，我还没写
    }
    return weight;
}





