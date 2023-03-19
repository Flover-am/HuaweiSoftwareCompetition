#ifndef HUAWEISOFTWARECOMPETITION_ROBOT_H
#define HUAWEISOFTWARECOMPETITION_ROBOT_H


class robot {
public:
    int ID{-1};
    int stationID{-1};
    int item{0};

    float tValue{0}; // timeValue
    float hValue{0}; // hitValue

    float angleV{0};
    float lineVX{0};
    float lineVY{0};

    float direction{0};
    float positionX;
    float positionY;

    robot(int, float, float);
    robot();
};


#endif //HUAWEISOFTWARECOMPETITION_ROBOT_H
