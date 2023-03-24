//
// Created by 18466 on 2023/3/21.
//

#ifndef CODECRAFTSDK_AVOIDCOLLISION_H
#define CODECRAFTSDK_AVOIDCOLLISION_H


#include <cmath>
#include "robot.h"
#include "data.h"

extern Logger logger;


class vector_line {
public:
    float first;
    float second;
    bool is_polar;

    vector_line(float fir, float sec) : first(fir), second(sec) {

    }

    explicit operator string() const {
        stringstream ss("");
        ss << "X= " << first << ", Y= " << second << endl;
        return ss.str();
    }


};

class vector_polar {
public:
    float first;
    float second;

    vector_polar(float fir, float sec) : first(fir), second(sec) {
        if (first < 0) {
//            logger.writeError("Vector_polar Rou <0 !");
        }
        if (second > PAI) {
            second -= 2 * PAI;
        } else if (second < -PAI) {
            second += 2 * PAI;
        }
        if (first < 0) {
            first *= -1;
            if (second > 0) {
                if (sec >= 0) {
                    sec -= PAI;
                } else {
                    sec += PAI;
                }
            }
        }
    }

    explicit operator string() const {
        stringstream ss("");
        ss << "ρ= " << first << ", θ= " << second / PAI << " π " << endl;
        return ss.str();
    }


};

class Coll_area {
public:
    float rou;
    float center_alpha;
    float min_alpha;
    float max_alpha;
    float r;

    Coll_area(float len, float _alpha, float _r) {
        rou = len;
        center_alpha = _alpha;
        r = _r;
        float beta = asin(r / rou);
        min_alpha = center_alpha - beta;
        max_alpha = center_alpha + beta;
    }

    // r 0
    bool in_area(vector_polar p_polar) {
        if (p_polar.second >= min_alpha && p_polar.second <= max_alpha
            || p_polar.second + 2 * PAI >= min_alpha && p_polar.second + 2 * PAI <= max_alpha
            || p_polar.second - 2 * PAI >= min_alpha && p_polar.second - 2 * PAI <= max_alpha) {
            float b = -2 * rou * cos(p_polar.second - center_alpha);
            float c = pow(rou, 2) - pow(r, 2);
            float res_min = (-1 * b - ::sqrt(pow(b, 2) - 4 * c)) / 2;
//            logger.writeInfo("b= " + to_string(b) + "  c= " + to_string(c) + "  res_min= " + to_string(res_min));
            if (p_polar.first > res_min) {
                return true;
            }
            return false;
        } else
            return false;
    }

    explicit operator string() const {
        stringstream ss("");
        ss << "ρ: " << rou << ", α= " << center_alpha / PAI << "π, min: " << min_alpha / PAI << "π,max: "
           << max_alpha / PAI << "π ,r: "
           << r << endl;
        return ss.str();
    }
};


class avoidCollision {
public:
    static vector_polar line2polar(vector_line line_v) {
        float v_len = sqrt(pow(line_v.first, 2) + pow(line_v.second, 2));
        float alpha = 0;
        if (line_v.first == 0) {
            if (line_v.second > 0) {
                alpha = PAI / 2;
            } else {
                alpha = -PAI / 2;
            }
        } else {
            alpha = atan(line_v.second / line_v.first);
            if (line_v.first < 0) {
                if (line_v.second > 0) {
                    alpha += PAI;
                } else {
                    alpha -= PAI;
                }
            }
        }
        return vector_polar{v_len, alpha};
    }

    static vector_line polar2line(vector_polar polar_v) {
        return vector_line{polar_v.first * cos(polar_v.second), polar_v.first * sin(polar_v.second)};
    }

    // va + vb

    static vector_polar polar_vec_add(vector_polar va_polar, vector_polar vb_polar) {
        vector_line va_line = polar2line(va_polar);
        vector_line vb_line = polar2line(vb_polar);

        return line2polar(vector_line{va_line.first + vb_line.first, va_line.second + vb_line.second});
    }

    // va - vb
    static vector_polar polar_vec_sub(vector_polar va_polar, vector_polar vb_polar) {
        vector_line va_line = polar2line(va_polar);
        vector_line vb_line = polar2line(vb_polar);

        return line2polar(vector_line{va_line.first - vb_line.first, va_line.second - vb_line.second});
    }

    static bool judgeColl(int robotNum) {
        stringstream ss("");
        bool to_return = false;
        int r_a_index = robotNum;
        for (int c = 0; c < 1; ++c) {
            for (int r_b_index = r_a_index + 1; r_b_index < 4; ++r_b_index) {
                // TODO: 统一角度到-π ~ π -- DONE
                robot ra = data::robots[r_a_index];
                robot rb = data::robots[r_b_index];

                ifstream prams("prams.in");
                float time = 1;
                float dis_deta = 1.3;
//                prams >> time >> dis_deta;
//                prams.close();
                if (data::frame == 99) {
                    logger.writeError(to_string(data::workStations.size()));
                }

                if (data::workStations.size() == 43) {
                    time = 1;
                    dis_deta = 1.3;
                }
                float dis_min = ra.r + rb.r + dis_deta;


                // 位置向量系
                vector_line Xab_line = {ra.positionX - rb.positionX, ra.positionY - rb.positionY};
                vector_polar Xab_polar = line2polar(Xab_line);
//                if (Xab_polar.first < dis_min - 0.3) {
//                    printf("forward %d %f\n", r_a_index, -1.0);
//                    printf("rotate %d %f\n", r_a_index, PAI);
//                    printf("forward %d %f\n", r_b_index, -1.0);
//                    printf("rotate %d %f\n", r_b_index, PAI / 2);
//                    to_return = true
//                    continue;
//                }
                // 速度向量系
                vector_line Vab_line = {rb.lineVX - ra.lineVX, rb.lineVY - ra.lineVY};
                vector_polar Vab_polar = line2polar(Vab_line);
                // 创建Ca|b 区域，以S/t为圆心，S的alpha为alpha
                Coll_area area = Coll_area(Xab_polar.first / time, Xab_polar.second, dis_min);
                if (/*area.in_area(Vab_polar)*/true) {

                    // 求robot的preference速度 V_pref(机器人脚下到目标工作台的位置)

                    int des_ws = data::destList[r_a_index][0].first;
                    vector_line ws_line{data::workStations[des_ws].positionX,
                                        data::workStations[des_ws].positionY};
                    vector_polar ro2ws_polar = line2polar(
                            vector_line{ws_line.first - ra.positionX, ws_line.second - ra.positionY});
                    vector_polar Va_pref = {6, ro2ws_polar.second};

                    des_ws = data::destList[r_b_index][0].first;
                    ws_line = {data::workStations[des_ws].positionX,
                               data::workStations[des_ws].positionY};
                    ro2ws_polar = line2polar(
                            vector_line{ws_line.first - rb.positionX, ws_line.second - rb.positionY});
                    vector_polar Vb_pref = {6, ro2ws_polar.second};

                    // Vab_pref_p
                    vector_polar Vab_pref_p = polar_vec_sub(Vb_pref, Va_pref);
                    vector_line Vab_pref_l = polar2line(Vab_pref_p);

                    if (!area.in_area(Vab_pref_p)) {
//                        logger.writeError("Vab_pref_p not in Ca|b", false);

                    }
                    Coll_area real_area = Coll_area(Xab_polar.first / time, Xab_polar.second, dis_min - 0.5);

                    /* 日志 */
//                    logger.writeInfo((string) ra);
//                    logger.writeInfo((string) rb);
//
//                    logger.writeInfo("Xab_polar");
//                    logger.writeInfo((string) Xab_polar);
//
//                    logger.writeInfo("Vab_line");
//                    logger.writeInfo((string) Vab_line);
//
//                    logger.writeInfo("Vab_polar");
//                    logger.writeInfo((string) Vab_polar);
//
//                    logger.writeInfo("Area:");
//                    logger.writeInfo((string) area);
//
//                    logger.writeInfo("Va_pref");
//                    logger.writeInfo((string) Va_pref);
//
//                    logger.writeInfo("Vb_pref");
//                    logger.writeInfo((string) Vb_pref);
//
//                    logger.writeInfo("Vab_pref");
//                    logger.writeInfo((string) Vab_pref_p);
//
//                    logger.writeInfo("Real_area");
//                    logger.writeInfo((string) real_area);
                    /* 日志 */
                    if (data::frame > 3000 && data::frame < 5000 && data::frame % 5 == 0) {
                        logger.writeInfo(to_string(data::frame));
                        logger.writeInfo(to_string(r_a_index) + "  " + to_string(r_b_index));
                        logger.writeInfo(to_string(area.in_area(Vab_polar)));
                        logger.writeInfo("Area:");
                        logger.writeInfo((string) area);
                        logger.writeInfo("Vab_polar");
                        logger.writeInfo((string) Vab_polar);
                    }
                    if (!area.in_area(Vab_polar)) {
                        goto out1;
                    }


                    // 求Vab_pref到 area边界最近的点
                    if (Vab_pref_p.second < area.center_alpha) {

                        //u 是速度向量
                        vector_polar u_p_half = {
                                Vab_pref_p.first * sin(Vab_pref_p.second - area.min_alpha) * 1 / 2,
                                area.min_alpha - PAI / 2};

                        vector_polar Va_result = polar_vec_sub(
                                Va_pref, u_p_half);
                        vector_polar Vb_result = polar_vec_add(
                                Vb_pref, u_p_half);
                        if (Va_result.first > 6) {
                            Va_result.first = 6;
                        }
                        if (Vb_result.first > 6) {
                            Vb_result.first = 6;
                        }

                        vector_polar Vab_result = polar_vec_sub(Vb_result, Va_result);
                        if (real_area.in_area(Vab_result)) {
//                            logger.writeError("Result still in Area???????? ");
                        }
//                        logger.writeInfo("Va_result");
//                        logger.writeInfo((string) Va_result);
//                        logger.writeInfo("Vb_result");
//                        logger.writeInfo((string) Vb_result);
//                        logger.writeInfo("Vab_result");
//                        logger.writeInfo((string) Vab_result);

                        float alpha = ra.alpha - Va_result.second;
                        alpha = (alpha > PAI) ? alpha - 2 * PAI : alpha;
                        alpha = (alpha < -PAI) ? alpha + 2 * PAI : alpha;
                        float omiga = (fabs(4 * alpha) <= PAI) ? 4 * alpha : PAI;
                        printf("forward %d %f\n", r_a_index, Va_result.first);
                        printf("rotate %d %f\n", r_a_index, omiga);
//                        logger.writeError("RESULT::");
//                        logger.writeError(to_string(Va_result.first));
//                        logger.writeError(to_string(omiga));

                        if (data::destList[r_a_index][0].first == (data::destList[r_b_index][0].first) ||
                            ra.stationID == rb.stationID) {
                            logger.writeInfo("123123123");

                            printf("forward %d -2\n", r_b_index);
                            printf("rotate %d 0\n", r_b_index);
                        } else {
                            float beta = rb.alpha - Vb_result.second;
                            beta = (beta > PAI) ? beta - 2 * PAI : beta;
                            beta = (beta < -PAI) ? beta + 2 * PAI : beta;
                            float omiga2 = (fabs(4 * beta) <= PAI) ? 4 * beta : PAI;
                            printf("forward %d %f\n", r_b_index, Vb_result.first);
                            printf("rotate %d %f\n", r_b_index, omiga2);

                        }
//                        logger.writeError("RESULT::");
//                        logger.writeError(to_string(Vb_result.first));
//                        logger.writeError(to_string(omiga2));

                    } else if (Vab_pref_p.second > area.center_alpha) {

                        vector_polar u_p_half = {
                                Vab_pref_p.first * sin(Vab_pref_p.second - area.min_alpha) * 1 / 2,
                                PAI / 2 - area.max_alpha};

                        vector_polar Va_result = polar_vec_sub(
                                Va_pref, u_p_half);
                        vector_polar Vb_result = polar_vec_add(
                                Vb_pref, u_p_half);
                        if (Va_result.first > 6) {
                            Va_result.first = 6;
                        }
                        if (Vb_result.first > 6) {
                            Vb_result.first = 6;
                        }
                        vector_polar Vab_result = polar_vec_sub(Vb_result, Va_result);
                        if (real_area.in_area(Vab_result)) {
//                            logger.writeError("Result still in Area???????? ");
                        }
//                        logger.writeInfo("Va_result");
//                        logger.writeInfo((string) Va_result);
//                        logger.writeInfo("Vb_result");
//                        logger.writeInfo((string) Vb_result);
//                        logger.writeInfo("Vab_result");
//                        logger.writeInfo((string) Vab_result);

                        float alpha = ra.alpha - Va_result.second;
                        alpha = (alpha > PAI) ? alpha - 2 * PAI : alpha;
                        alpha = (alpha < -PAI) ? alpha + 2 * PAI : alpha;
                        float omiga = (fabs(4 * alpha) <= PAI) ? 4 * alpha : PAI;
                        printf("forward %d %f\n", r_a_index, Va_result.first);
                        printf("rotate %d %f\n", r_a_index, omiga);
//                        logger.writeError("RESULT::");
//                        logger.writeError(to_string(Va_result.first));
//                        logger.writeError(to_string(omiga));
                        if (data::destList[r_a_index][0].first == (data::destList[r_b_index][0].first) ||
                            ra.stationID == rb.stationID) {
                            logger.writeInfo("123123123");

                            printf("forward %d -2\n", r_b_index);
                            printf("rotate %d 0\n", r_b_index);
                        } else {
                            float beta = rb.alpha - Vb_result.second;
                            beta = (beta > PAI) ? beta - 2 * PAI : beta;
                            beta = (beta < -PAI) ? beta + 2 * PAI : beta;
                            float omiga2 = (fabs(4 * beta) <= PAI) ? 4 * beta : PAI;
                            printf("forward %d %f\n", r_b_index, Vb_result.first);
                            printf("rotate %d %f\n", r_b_index, omiga2);

                        }
//                        logger.writeError("RESULT::");
//                        logger.writeError(to_string(Vb_result.first));
//                        logger.writeError(to_string(omiga2));
                    }
                    // TODO: 新的navigate 来控制a，b，往目标速度Va，Vb移动
                    to_return = true;
                } else {
                }
                out1:
                int a = 1;
            }
        }

        return to_return;
    }
};

#endif //CODECRAFTSDK_AVOIDCOLLISION_H
