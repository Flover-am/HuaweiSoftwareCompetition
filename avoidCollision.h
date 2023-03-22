//
// Created by 18466 on 2023/3/21.
//

#ifndef CODECRAFTSDK_AVOIDCOLLISION_H
#define CODECRAFTSDK_AVOIDCOLLISION_H
#define NAME_OF(v) #v

#include <cmath>
#include "robot.h"
#include "data.h"

extern Logger logger;


class vector_me {
public:
    float first;
    float second;

    vector_me(float fir, float sec) : first(fir), second(sec) {
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
        ss << "①: " << first << ", ②: " << second << endl;
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
    bool in_area(vector_me p_polar) {
        if (p_polar.second >= min_alpha && p_polar.second <= max_alpha) {
            float b = 2 * rou * cos(p_polar.second - center_alpha);
            float c = pow(rou, 2) - pow(r, 2);
            float deta = (-1 * b - ::sqrt(pow(b, 2) - 4 * c)) / 2;
            if (p_polar.first > deta) {
                return true;
            }
            return false;
        } else
            return false;
    }

    explicit operator string() const {
        stringstream ss("");
        ss << "rou: " << rou << ", alpha: " << center_alpha << ", min: " << min_alpha << ",max: " << max_alpha << ",r: "
           << r << endl;
        return ss.str();
    }
};


class avoidCollision {
public:
    static vector_me line2polar(vector_me line_v) {
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
            if (line_v.first < 0) {}
            alpha *= -1;
        }
        return vector_me{v_len, alpha};
    }

    static vector_me polar2line(vector_me polar_v) {
        return vector_me{polar_v.first * cos(polar_v.second), polar_v.first * sin(polar_v.second)};
    }

    // va + vb
    vector_me mi = {1, 2};

    static vector_me polar_vec_add(vector_me va_polar, vector_me vb_polar) {
        vector_me va_line = polar2line(va_polar);
        vector_me vb_line = polar2line(vb_polar);

        return line2polar(vector_me{va_line.first + vb_line.first, va_line.second + vb_line.second});
    }

    // va - vb
    static vector_me polar_vec_sub(vector_me va_polar, vector_me vb_polar) {
        vector_me va_line = polar2line(va_polar);
        vector_me vb_line = polar2line(vb_polar);

        return line2polar(vector_me{va_line.first - vb_line.first, va_line.second - vb_line.second});
    }

    static bool judgeColl() {
        stringstream ss("");
        for (int r_a_index = 0; r_a_index < 4; ++r_a_index) {
            for (int r_b_index = r_a_index + 1; r_b_index < 4; ++r_b_index) {
                if (r_a_index == r_b_index) {
                    break;
                }
                // TODO: 统一角度到-π ~ π -- DONE
                float time = 0.1;
                robot ra = data::robots[r_a_index];
                robot rb = data::robots[r_b_index];
                float dis_min = ra.r + rb.r + 0.3;


                // 位置向量系
                vector_me Xab_line = {ra.positionX - rb.positionX, ra.positionY - rb.positionY};
                vector_me Xab_polar = line2polar(Xab_line);
                // 速度向量系
                vector_me Vab_line = {ra.lineVX - rb.lineVX, ra.lineVY - rb.lineVY};
                vector_me Vab_polar = line2polar(Vab_line);
                // 创建Ca|b 区域，以S/t为圆心，S的alpha为alpha
                Coll_area area = Coll_area(Xab_polar.first / time, Xab_polar.second, dis_min);
                if (area.in_area(Vab_polar)) {

                    // 求robot的preference速度 V_pref(机器人脚下到目标工作台的位置)

                    int des_ws = data::destList[r_a_index][0].first;
                    vector_me ws_line{data::workStations[des_ws].positionX,
                                      data::workStations[des_ws].positionY};
                    vector_me ro2ws_polar = line2polar(
                            vector_me{ws_line.first - ra.positionX, ws_line.second - ra.positionY});
                    vector_me Va_pref = {6, ro2ws_polar.second};

                    des_ws = data::destList[r_b_index][0].first;
                    ws_line = {data::workStations[des_ws].positionX,
                               data::workStations[des_ws].positionY};
                    ro2ws_polar = line2polar(
                            vector_me{ws_line.first - rb.positionX, ws_line.second - rb.positionY});
                    vector_me Vb_pref = {6, ro2ws_polar.second};

                    // Vab_pref_p
                    vector_me Vab_pref_p = polar_vec_sub(Va_pref, Vb_pref);
                    vector_me Vab_pref_l = polar2line(Vab_pref_p);

                    if (!area.in_area(Vab_pref_p)) {
                        logger.writeError("Vab_pref_p not in Ca|b", false);

                    }

                    /* 日志 */
                    logger.writeInfo((string) ra);
                    logger.writeInfo((string) rb);
                    logger.writeInfo("Vab_polar", false);
                    logger.writeInfo((string) Vab_polar, false);
                    logger.writeInfo("Va_pref", false);
                    logger.writeInfo((string) Va_pref, false);
                    logger.writeInfo("Vb_pref", false);
                    logger.writeInfo((string) Vb_pref, false);
                    logger.writeInfo("Vab_pref", false);
                    logger.writeInfo((string) Vab_pref_p, false);
                    logger.writeInfo((string) area, false);
                    /* 日志 */

                    // 求Vab_pref到 area边界最近的点
                    if (Vab_pref_p.second < area.center_alpha) {


                        vector_me u_p_half = {
                                Vab_pref_p.first * sin(Vab_pref_p.second - area.min_alpha) * 3 / 5,
                                area.min_alpha - PAI / 2};

                        vector_me Va_result = polar_vec_add(
                                line2polar(vector_me{ra.lineVX, ra.lineVY}), u_p_half);
                        vector_me Vb_result = polar_vec_sub(
                                line2polar(vector_me{rb.lineVX, rb.lineVY}), u_p_half);
                    } else if (Vab_pref_p.second > area.center_alpha) {

                        vector_me u_p_half = {
                                Vab_pref_p.first * sin(Vab_pref_p.second - area.min_alpha) * 3 / 5,
                                PAI / 2 - area.max_alpha};

                        vector_me Va_result = polar_vec_add(
                                line2polar(vector_me{ra.lineVX, ra.lineVY}), u_p_half);
                        vector_me Vb_result = polar_vec_sub(
                                line2polar(vector_me{rb.lineVX, rb.lineVY}), u_p_half);
                    }

                    // TODO: 新的navigate 来控制a，b，往目标速度Va，Vb移动
//                    return true;

                } else {
//                    return false;
                }
            }
        }
        return false;
    }
};

#endif //CODECRAFTSDK_AVOIDCOLLISION_H
