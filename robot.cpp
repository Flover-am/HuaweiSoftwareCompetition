//
// Created by 24873 on 2023/3/14.
//

#include "robot.h"

robot::robot(float x, float y) {
    this->position_x = x;
    this->position_y = y;
}

robot::robot() {
    this->position_x = 0;
    this->position_y = 0;
}
