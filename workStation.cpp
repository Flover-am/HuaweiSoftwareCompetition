//
// Created by 18466 on 2023/3/15.
//

#include "workStation.h"
#include "iostream"

workStation::workStation(int _type, int _id, float x, float y) : position_x(x), position_y(y) {
    id = _id;
    type = _type;
    switch (type) {

    }
}

workStation::workStation() : position_x(0), position_y(0) {
    id = -1;
    type = 1;
}

std::string workStation::to_string() {
    std::string res = "WorkStation[" + std::to_string(id) + "]: (1)type:" + std::to_string(type) + "    (2)position:( " +
                      std::to_string(position_x) + "," + std::to_string(position_y);
    return res;
}
