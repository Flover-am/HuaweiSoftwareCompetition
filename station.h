#ifndef CODECRAFTSDK_STATION_H
#define CODECRAFTSDK_STATION_H

#include <array>
#include <string>
#include <vector>

#define SHORT_TIME 50
#define MEDIUM_TIME 500
#define LONG_TIME 1000

using namespace std;

struct Ask{
    int RID{-1};
    int timeStamp{-1};
    double value{-1};
};

class station {
public:

    const int id{-1};
    const int type{0};
    const double positionX{0.0};
    const double positionY{0.0};

    int timeRemain{-1};
    int proState{0};
    array<int ,8> matState{0};
    // 各个端口可用的时间戳，-1代表不可用，[0]代表成品端口
    array<int, 8> portAvailableTime{-1, -1, -1, -1, -1, -1, -1, -1};
    // 各个端口将接收到对应机器人买卖请求的时间戳
    array<vector<Ask>, 8> portRecvAskTime;

    station(int, int, double, double);
    station();

    string to_string();
};

#endif //CODECRAFTSDK_STATION_H
