#ifndef CODECRAFTSDK_STATION_H
#define CODECRAFTSDK_STATION_H

#include <array>
#include <string>

using namespace std;

class station {
public:
    int id{};
    int type{1};
    const float positionX{0.0f};
    const float positionY{0.0f};

    // 各个端口可用的时间戳，-1代表不可用，[0]代表成品端口
    array<int, 8> portAvailableTime{-1, -1, -1, -1, -1, -1, -1, -1};
    station(int, int, float, float);
    station();

    // TODO：数据更新
    void refresh();
    string to_string();
};


#endif //CODECRAFTSDK_STATION_H
