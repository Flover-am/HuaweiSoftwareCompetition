#include "workStation.h"
#include "iostream"

workStation::workStation(int _type, int _id, float x, float y):
    position_x(x), position_y(y), id(_id), type(_type) {}
workStation::workStation(){};

string workStation::to_string(){
    string res =
            "WorkStation[" + std::to_string(id) + "]" +
            "(1)type:" + std::to_string(type) + "   " +
            "(2)position:( " + std::to_string(position_x) + "," + std::to_string(position_y);
    return res;
}
