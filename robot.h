//
// Created by 24873 on 2023/3/14.
//

#ifndef HUAWEISOFTWARECOMPETITION_ROBOT_H
#define HUAWEISOFTWARECOMPETITION_ROBOT_H


class robot {
public:
    //角速度
    float angle_v{};
    //线速度
    float line_v_x{};
    float line_v_y{};
    //位置
    float position_x{};
    float position_y{};

    robot(float, float);

    robot();
};


#endif //HUAWEISOFTWARECOMPETITION_ROBOT_H
