//
// Created by 18466 on 2023/3/15.
//

#ifndef CODECRAFTSDK_WORKSTATION_H
#define CODECRAFTSDK_WORKSTATION_H

#include <vector>
#include "string"

class workStation {
public:
    int id{};
    int type{1};
    const float position_x;
    const float position_y;

    std::vector<int> buy_list;
    // TODO: 待加入其他工作台元素
    int sell_material{0};
    int frame_need{0};
    float frame2finished{0};

    workStation(int, int, float, float);

    workStation();
    // TODO：数据更新
    void refresh();

    std::string to_string();
};


#endif //CODECRAFTSDK_WORKSTATION_H
