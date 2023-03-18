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

    // TODO: 待加入其他工作台元素

    int frame_need{0};
    int frame2finished{0};
    // 第一位表示原材料种类，第二位表示原材料状态
    std::vector<std::vector<int>> raw_carried_status;
    // 同理
    std::vector<int> sell_carried_status;

    workStation(int, int, float, float);

    workStation();

    // TODO：数据更新
    void refresh();

    std::string to_string();
};


#endif //CODECRAFTSDK_WORKSTATION_H
