#include "pathPlanning.h"
void selectPath(int RID){
    auto &pathTree = data::pathTrees[RID];
    vector<float> weights;
    vector<bool> available;
    // TODO: 刷新权值
    int width = pathTree[STEP_DEPTH-1].size();
    // 冲突检测
    for (int i = 0; i < width; ++i){
        bool conflict = false;
        int index = i;
        for (int depth = STEP_DEPTH-1; depth >= 0; --depth){
            auto &step = pathTree[depth][index];
            if (step.OID == ONLY_SELL)  // 如果只卖，不会发生冲突
                continue;

            for (int robotNum = 0; robotNum < ROBOT_NUM; ++robotNum){ // 对每个其他的机器人
                if (robotNum == RID)
                    continue;

                int _index = data::pathNums[robotNum];  // 选中机器人的当前路径
                for (int _depth = STEP_DEPTH-1; _depth >= 0; --_depth){
                    auto &_step = pathTree[_depth][_index];
                    if (_step.SID == step.SID && _step.OID == _step.SID){
                        conflict = true;
                        goto out;
                    }          // 如果发生冲突
                    _index = _step.lastIndex;
                }

            }
            index = step.lastIndex;
        }
        out :
        available.emplace_back(!conflict);
    }
    // 将权值加和
    for (int i = 0; i < width; ++i){
        float weight = 0;
        int index = i;
        for (int depth = STEP_DEPTH-1; depth >= 0; --depth){
            weight += pathTree[depth][index].value;
            index = pathTree[depth][index].lastIndex;
        }
        weights.emplace_back(weight);
    }
    // 选择
    for (int i = 0; i < width; ++i)
        if (available[i] && weights[i] < weights[data::pathNums[RID]])
            data::pathNums[RID] = i;
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
                    pathTree[depth-1][lastIndex].nextIndex.emplace_back(index);
                    index++;
                }
                lastIndex++;
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
                pair<float, float> value = calculateValue(RID, s.id, ONLY_BUY, firstStep);
                int timeStamp = (int)value.first+data::frame;
                vector<int> newNext;
                Step newStep{s.id, ONLY_BUY, timeStamp, value.second, lastIndex, newNext};
                stations.emplace_back(newStep);
            }
    }
    else{
        for (int &SID : data::receiveStationIDs[IID]) {
            auto &s = data::stations[SID];
            if (s.type == 8 || s.type == 9){
                pair<float, float> value = calculateValue(RID, s.id, ONLY_SELL, firstStep);
                int timeStamp = (int)value.first+data::frame;
                vector<int> newNext;
                Step newStep{s.id, ONLY_SELL, timeStamp, value.second, lastIndex, newNext};
                stations.emplace_back(newStep);
            }

            else{
                pair<float, float> value = calculateValue(RID, s.id, IID, firstStep);
                int timeStamp = (int)value.first+data::frame;
                vector<int> newNext;
                Step newStep{s.id, IID, timeStamp, value.second, lastIndex, newNext};
                stations.emplace_back(newStep);
            }
        }
    }
    // TODO: 算法优化
    return stations;
}
pair<float, float> calculateValue(int RID, int SID, int OID, bool firstStep){
    station &s = data::stations[SID];
    pair<float, float> value;
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
    value.second = value.first/value.second;
    return value;
}




