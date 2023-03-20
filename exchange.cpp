#include "exchange.h"

void exchange(int RID, pair<int, int> SID){
    robot &r = data::robots[RID];
    workStation &s = data::workStations[SID.first];

    bool success = false;               // 由于可能需要等待，需要判定是否操作成功

    if (SID.second != ONLY_BUY){
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

    if (success){
        for (int i = STEP_DEPTH - 1; i > 0; --i)
            data::destList[RID][i-1] = data::destList[RID][i];
        pair<int, int> element(-1, -1);
        data::destList[RID][STEP_DEPTH - 1] = element;
    }
}