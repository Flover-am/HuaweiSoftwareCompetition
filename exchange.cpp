#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-narrowing-conversions"

#include "exchange.h"

void exchange(int RID, int SID, int OID){
    Robot &r = data::robots[RID];
    Station &s = data::stations[SID];

    bool success = false;               // 由于可能需要等待，需要判定是否操作成功

    if (OID != ONLY_BUY){
        // 当前帧对应端口的第一个请求不是-1号（没有物品，可卖）
        if (s.condition[0][r.item][0] != -1){
            printf("sell %d\n", RID);
            success = true;
        }
    }
    if (!success){
        // 当前帧对应的生产物品量不为0，可买
        if (s.prosNum[0] > 0){
            printf("buy %d\n", RID);    // 如果可买，则购进原料
            success = true;
        }
    }
    if (success)
        pushStep(RID);
}
void pushStep(int RID){
    array<vector<Step>, STEP_DEPTH> &pathTree = data::pathTrees[RID];
    int stepID = data::optedPaths[RID][0];

    // 计算每层路径保留的范围的距离
    array<int, STEP_DEPTH> beginIndex{};
    array<int, STEP_DEPTH> endIndex{};

    for (int depth = 0; depth < STEP_DEPTH; ++depth) {
        int width = pathTree[depth].size();
        endIndex[depth] = width;
        bool start = false;
        for (int i = 0; i < width; ++i){

            int ancestorIndex = i;                  // 找到这个节点的祖先
            for (int k = depth; k > 0; --k)
                ancestorIndex = pathTree[k][ancestorIndex].lastIndex;

            if (!start && ancestorIndex == stepID) {
                start = true;
                beginIndex[depth] = i;
            }
            if ( start && ancestorIndex != stepID) {
                endIndex[depth] = i;
                break;
            }
        }
    }
    // 构建新树
    array<vector<Step>, STEP_DEPTH> newTree;
    for (int depth = 0; depth < STEP_DEPTH; ++depth) {
        vector<Step> newSteps;
        for (int i = beginIndex[depth]; i < endIndex[depth]; ++i){
            Step newStep = pathTree[depth][i];
            if (depth > 0)
                newStep.lastIndex -= beginIndex[depth-1];
            if (depth < STEP_DEPTH-1)
                for (int &next : newStep.nextIndex)
                    next -= beginIndex[depth+1];
            newSteps.emplace_back(newStep);
        }
        newTree[depth] = newSteps;
    }

    // 计算valueSum
    for (int depth = 1; depth < STEP_DEPTH; ++depth)
        for (auto &step : newTree[depth])
            step.valueSum -= newTree[0][0].value;

    for (int depth = 0; depth < STEP_DEPTH-1; ++depth)
        newTree[depth] = newTree[depth+1];
    newTree[STEP_DEPTH-1].clear();
    pathTree = newTree;
    if (data::frame == 4730){
        for (auto &steps : pathTree){
            string s;
            for (auto &step : steps){
                s += to_string(step.SID) + " ";
            }
            logger.writeInfo(s);
        }
    }
}
#pragma clang diagnostic pop