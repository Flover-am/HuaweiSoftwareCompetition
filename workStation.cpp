//
// Created by 18466 on 2023/3/15.
//

#include "workStation.h"
#include "iostream"

workStation::workStation(int _type, int _id, float x, float y) : position_x(x), position_y(y) {
    id = _id;
    type = _type;

    switch (type) {
        case 1:
            raw_carried_status = {};
            sell_carried_status = {1, 0};
            frame_need = 50;
            frame2finished = -1;
            break;
        case 2:
            raw_carried_status = {};
            sell_carried_status = {2, 0};
            frame_need = 50;
            frame2finished = -1;
            break;
        case 3:
            raw_carried_status = {};
            sell_carried_status = {3, 0};
            frame_need = 50;
            frame2finished = -1;
            break;
        case 4:
            raw_carried_status = {{1, 0},
                                  {2, 0}};
            sell_carried_status = {4, 0};
            frame_need = 500;
            frame2finished = -1;
            break;
        case 5:
            raw_carried_status = {{1, 0},
                                  {3, 0}};
            sell_carried_status = {5, 0};
            frame_need = 500;
            frame2finished = -1;
            break;
        case 6:
            raw_carried_status = {{2, 0},
                                  {3, 0}};
            sell_carried_status = {6, 0};
            frame_need = 500;
            frame2finished = -1;
            break;
        case 7:
            raw_carried_status = {{4, 0},
                                  {5, 0},
                                  {6, 0}};
            sell_carried_status = {7, 0};
            frame_need = 1000;
            frame2finished = -1;
            break;
        case 8:
            raw_carried_status = {{7, 0}};
            sell_carried_status = {};
            frame_need = 1;
            frame2finished = -1;
            break;
        case 9:
            raw_carried_status = {{1, 0},
                                  {2, 0},
                                  {3, 0},
                                  {4, 0},
                                  {5, 0},
                                  {6, 0},
                                  {7, 0}};
            sell_carried_status = {};
            frame_need = 1;
            frame2finished = -1;
            break;
    }
}

workStation::workStation() : position_x(0), position_y(0) {
    id = -1;
    type = 1;
}

std::string workStation::to_string() {
    std::string res =
            "WorkStation[" + std::to_string(id) + "]: (1)type:" + std::to_string(type) + "    (2)position:( " +
            std::to_string(position_x) + "," + std::to_string(position_y);
    return res;
}
