#ifndef CODECRAFTSDK_WORKSTATION_H
#define CODECRAFTSDK_WORKSTATION_H

#include <vector>
#include <string>

using namespace std;

class workStation {
public:
    int id{};
    int type{1};
    const float positionX{0.0f};
    const float positionY{0.0f};

    int matState[7]{0,0,0,0,0,0,0}; // matState[i] == 1: i号原料格占用
    int proState{0};                // proState == 1: 产品格占用
    int timeRemain{-1};             // 生产剩余所需帧数

    workStation(int, int, float, float);
    workStation();

    // TODO：数据更新
    void refresh();
    string to_string();
};


#endif //CODECRAFTSDK_WORKSTATION_H
