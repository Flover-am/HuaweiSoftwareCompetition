#include "conflictRecall.h"

void detectConflict(int RID, int item) {
    auto &thisStep = data::pathTrees[RID][STEP_DEPTH - 1];

    bool canSell = false;
    for (int &SID : data::receiveStationIDs[item]) {
        auto &sellS = data::stations[SID];
        // 判定潜在的卖出冲突
        for (int robotNum = 0; robotNum < ROBOT_NUM; ++robotNum) {
            if (robotNum == RID)
                continue;
            for (auto &step: data::pathTrees[robotNum])
                if (step.first == sellS.id)                                   // 若去同一个工作台
                    if (step.second == item && step.second != ONLY_SELL)      // 若有冲突
                        goto out;
        }
            // 如果场上有可卖的工作台
        if (sellS.matState[item] == 0 /*|| (ss.timeRemain != -1 且 原料台均已满)*/) {  // 如果场上有可卖的工作台
            canSell = true;
            break;
        }
        out: continue;
    }
    if (!canSell){
        string s = "recall " + to_string(RID) + " buy " + to_string(item) + " at: " + to_string(data::frame);
        logger.writeInfo(s, false);
        thisStep.first = -1;
        thisStep.second = -1;
    }
}