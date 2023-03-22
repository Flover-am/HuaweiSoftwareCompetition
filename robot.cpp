#include "robot.h"

robot::robot(int _id, double x, double y) {
    this->ID = _id;
    this->positionX = x;
    this->positionY = y;
}

robot::robot() {
    this->positionX = 0;
    this->positionY = 0;
}
