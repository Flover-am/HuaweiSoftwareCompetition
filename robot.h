#ifndef HUAWEISOFTWARECOMPETITION_ROBOT_H
#define HUAWEISOFTWARECOMPETITION_ROBOT_H

#define V_MAX 6.0
#define OMEGA_MAX 3.14159
#define R_SMALL 0.45
#define M_SMALL 12.72345
#define A_SMALL 19.64875
#define ALPHA_SMALL 38.81236
#define R_BIG 0.53
#define M_BIG 17.64947
#define A_BIG 14.16473
#define ALPHA_BIG 20.17050

//操作编号，对应商品编号，也对应工作台端口号
#define ONLY_BUY 0
#define PROD_SELL_1 1
#define PROD_SELL_2 2
#define PROD_SELL_3 3
#define PROD_SELL_4 4
#define PROD_SELL_5 5
#define PROD_SELL_6 6
#define ONLY_SELL 7

class robot {
public:
    int ID{-1};
    int stationID{-1};
    int item{0};

    double r{R_SMALL};
    double m{M_SMALL};
    double a{A_SMALL};
    double alpha{ALPHA_SMALL};

    double tValue{0}; // timeValue
    double hValue{0}; // hitValue

    double angleV{0};
    double lineVX{0};
    double lineVY{0};

    double direction{0};
    double positionX;
    double positionY;

    robot(int, double, double);
    robot();
};

#endif //HUAWEISOFTWARECOMPETITION_ROBOT_H
