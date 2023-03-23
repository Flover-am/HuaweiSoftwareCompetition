#include "station.h"

Station::Station(int _type, int _id, float x, float y):
        type(_type), id(_id), positionX(x), positionY(y) {}
Station::Station()=default;

string Station::to_string(){
    string res =
            "WorkStation[" + std::to_string(id) + "]" +
            "(1)type:" + std::to_string(type) + "   " +
            "(2)position:( " + std::to_string(positionX) + "," + std::to_string(positionY);
    return res;
}
