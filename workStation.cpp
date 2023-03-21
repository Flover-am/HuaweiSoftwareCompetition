#include "workStation.h"
#include "iostream"

workStation::workStation(int _type, int _id, float x, float y):
        type(_type), id(_id), positionX(x), positionY(y) {}
workStation::workStation(){};

string workStation::to_string(){
    string res =
            "WorkStation[" + std::to_string(id) + "]" +
            "(1)type:" + std::to_string(type) + "   " +
            "(2)position:( " + std::to_string(positionX) + "," + std::to_string(positionY);
    return res;
}
