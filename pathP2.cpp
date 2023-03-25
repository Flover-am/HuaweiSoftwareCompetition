//
// Created by 18466 on 2023/3/21.
//

#include "pathP2.h"


int find_max(int begin, int depth) {
    int workStaNum = data::workStations.size();
    float max_weight = 0;
    int des = 0;
    if (depth == 1) {
        for (int i = 0; i < workStaNum; ++i) {
            if (max_weight < data::weight[begin - 1][i][0]) {
                max_weight = data::weight[begin - 1][i][0];
                des = i;
            }
        }
        return des + 1;
    }
    if (depth == 2) {
        for (int i = 0; i < workStaNum; ++i) {
            if (max_weight < data::weight[begin - 1][i][1]) {
                max_weight = data::weight[begin - 1][i][1];
                des = i;
            }
        }
        return des + 1;
    }
    return 1;
}

void pathP2::initweight() {
    int workStationNum = data::workStations.size();
    // one step weight
    for (int i = 0; i < workStationNum; ++i) {
        for (int j = 0; j < workStationNum; ++j) {
            auto wS1 = data::workStations[i];
            auto wS2 = data::workStations[j];

            int to_sell_int = data::materialEachTypeWay[i][j] - 1;
            material to_sell = material(to_sell_int + 1);

            float profits = (float) (to_sell.sellPrice - to_sell.costPrice);
            float time_need = data::distance[i][j] / V_MAX /*+ data::workStations[j].timeRemain*/;

            if (wS2.matState[to_sell_int] == 1) {
                data::weight[i][j][0] = 0;
            } else {
                data::weight[i][j][0] = profits / time_need;
            }
        }

    }
    // two step weight
    for (int i = 0; i < workStationNum; ++i) {
        for (int j = 0; j < workStationNum; ++j) {
            auto wS1 = data::workStations[i];
            auto wS2 = data::workStations[j];
            for (int k = 0; k < workStationNum; ++k) {
                if (data::materialEachTypeWay[j][k] != 0)
                    data::weight[i][j][1] == data::weight[j][k][0];
            }
        }
    }
}

int pathP2::setDes(robot &r) {
    int workS_1 = r.stationID;
    if (r.item != 0) {
        int one_step_D = find_max(workS_1, 1);
        return one_step_D;
    } else {
        int two_step_D = find_max(workS_1, 1);
        return two_step_D;
    }
}
