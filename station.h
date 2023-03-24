#ifndef CODECRAFTSDK_STATION_H
#define CODECRAFTSDK_STATION_H

#include <array>
#include <string>
#include <deque>

#define SHORT_TIME 50
#define MEDIUM_TIME 500
#define LONG_TIME 1000
#define UNAVAILABLE 1000

using namespace std;

struct Ask{
    int RID{-1};
    int OID{-1};
    int timeStamp{-1};
    float value{-1};
};

class Station {
public:

    const int id{-1};
    const int type{0};
    const float positionX{0.0};
    const float positionY{0.0};

    // 各个工作台将接收到请求
    deque<Ask> asks;
    // 各个端口在各个时间戳的请求状态，每个vector元素是请求编号
    array<array<deque<int>, 8>, UNAVAILABLE> condition;
    // 工作台剩余工作时间
    array<int, UNAVAILABLE> frameRemain;
    // 工作台生产产品数量
    array<int, UNAVAILABLE> prosNum;

    Station(int, int, float, float);
    Station();
    string to_string();
};

#endif //CODECRAFTSDK_STATION_H
