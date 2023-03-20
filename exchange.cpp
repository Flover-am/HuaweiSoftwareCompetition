#include "exchange.h"

void exchange(int RID, int SID){
    robot &r = dataTable::robots[RID];
    workStation &s = dataTable::workStations[SID];

    bool success = false;           // 由于可能需要等待，需要判定是否操作成功
    if (r.item != 0 && s.matState[r.item] == 0){
        printf("sell %d\n", RID);   // 如果可卖，则卖出物品
        success = true;
    }
    if (success || (r.item == 0 && s.proState == 1)){
        printf("buy %d\n", RID);    // 如果可买，则购进原料
        success = true;
    }
    if (success){
        dataTable::destList[RID] = -1;
    }
}