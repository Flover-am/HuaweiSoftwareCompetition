#include "station.h"

station::station(int _type, int _id, double x, double y):
        type(_type), id(_id), positionX(x), positionY(y) {}
station::station()=default;

string station::to_string(){
    string res =
            "WorkStation[" + std::to_string(id) + "]" +
            "(1)type:" + std::to_string(type) + "   " +
            "(2)position:( " + std::to_string(positionX) + "," + std::to_string(positionY);
    return res;
}
