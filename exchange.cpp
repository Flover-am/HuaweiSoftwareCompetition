#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-narrowing-conversions"

#include "exchange.h"

void exchange(int RID, int SID, int OID){
    robot &r = data::robots[RID];
    station &s = data::stations[SID];

    bool success = false;               // 由于可能需要等待，需要判定是否操作成功

    if (OID != ONLY_BUY){
        if (s.matState[r.item] == 0){
            printf("sell %d\n", RID);   // 如果可卖，则卖出物品
            success = true;
        }
    }
    if (!success){
        if (s.proState == 1){
            printf("buy %d\n", RID);    // 如果可买，则购进原料
            success = true;
        }
    }

    if (success)
        pushStep(RID);
}
void pushStep(int RID){
    array<vector<Step>, STEP_DEPTH> &pathTree = data::pathTrees[RID];
    for (int depth = 0; depth < STEP_DEPTH-1; ++depth)
        pathTree[depth] = pathTree[depth + 1];
    pathTree[STEP_DEPTH-1].clear();

    //剪枝
    int width = 1;
    for (int depth = 0; depth < STEP_DEPTH-1; ++depth){
        int size = pathTree[depth].size();
        for (int i = width; i < size; ++i)
            pathTree[depth].pop_back();
        auto nextIndex = pathTree[depth][width - 1].nextIndex;
        width = nextIndex[nextIndex.size()-1];
    }
}
#pragma clang diagnostic pop