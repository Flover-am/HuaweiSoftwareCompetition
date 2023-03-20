#ifndef HUAWEISOFTWARECOMPETITION_ROBOT_H
#define HUAWEISOFTWARECOMPETITION_ROBOT_H

#define V_MAX 6.0f
#define OMEGA_MAX 3.14159f
#define R_SMALL 0.45f
#define M_SMALL 12.72345f
#define A_SMALL 19.64875f
#define ALPHA_SMALL 38.81236f
#define R_BIG 0.53f
#define M_BIG 17.64947f
#define A_BIG 14.16473f
#define ALPHA_BIG 20.17050f

class robot {
public:
    int ID{-1};
    int stationID{-1};
    int item{0};

    float r{R_SMALL};
    float m{M_SMALL};
    float a{A_SMALL};
    float alpha{ALPHA_SMALL};

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
