#include "robot.h"

Robot::Robot(int _id, float x, float y) {
    this->ID = _id;
    this->positionX = x;
    this->positionY = y;
}

Robot::Robot() {
    this->positionX = 0;
    this->positionY = 0;
}
