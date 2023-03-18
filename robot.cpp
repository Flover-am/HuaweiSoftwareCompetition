#include "robot.h"

robot::robot(int _id, float x, float y) {
    this->id = _id;
    this->positionX = x;
    this->positionY = y;
}

robot::robot() {
    this->positionX = 0;
    this->positionY = 0;
}
