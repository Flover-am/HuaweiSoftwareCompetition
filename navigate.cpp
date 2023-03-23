#include "navigate.h"

void navigate(int RID, int SID){
    Robot &r = data::robots[RID];
    Station &s = data::stations[SID];
    float rx = r.positionX, ry = r.positionY;
    float sx = s.positionX, sy = s.positionY;
    //double rv = sqrt(r.lineVX*r.lineVX+r.lineVY*r.lineVY);
    //double angleV = r.angleV;

    float distance = sqrt((sy-ry)*(sy-ry)+(sx-rx)*(sx-rx));
    float angle = atan2((sy-ry), (sx-rx));
    float alpha = angle-r.direction;
    if (alpha >  PAI)   alpha -= 2*PAI;
    if (alpha < -PAI)   alpha += 2*PAI;

    float omega, v = -2; // 默认后退

    if (alpha > 0)  omega = OMEGA_MAX >  FRAMES_PER_S*alpha ? FRAMES_PER_S*alpha :  OMEGA_MAX;
    else            omega = OMEGA_MAX > -FRAMES_PER_S*alpha ? FRAMES_PER_S*alpha : -OMEGA_MAX;

    if (abs(alpha) <= PAI/2){
        float cosAlpha = cos(alpha), cosDelta = cos(alpha+omega*TIME_PER_FRAME);
        float tmp = (sqrt((1-cosAlpha*cosAlpha)/(1-cosDelta*cosDelta)));
        float maxV_ = distance*(cosAlpha-cosDelta*tmp); // 根据方程计算v的最大值

        if (!isnan(maxV_) && maxV_ < V_MAX) v = maxV_;
        else                                v = V_MAX;

        // 是否需要为碰撞半径留出空余？？
        if (r.direction > 0) {
            float maxV = (50-r.r-ry)*FRAMES_PER_S/sin(r.direction);
            if (v > maxV) v = maxV;
        }
        else if (r.direction < 0) {
            float maxV = (r.r-ry)*FRAMES_PER_S/sin(r.direction);
            if (v > maxV) v = maxV;
        }
        if (r.direction < PAI/2 && r.direction > -PAI/2) {
            float maxV = (50-r.r-rx)*FRAMES_PER_S/cos(r.direction);
            if (v > maxV) v = maxV;
        }
        else if (r.direction < -PAI/2 || r.direction > PAI/2) {
            float maxV = (r.r-rx)*FRAMES_PER_S/cos(r.direction);
            if (v > maxV) v = maxV;
        }
    }
    printf("rotate %d %f\n", RID, omega);
    printf("forward %d %f\n", RID, v);
    //TODO:倒行情况
}

float calculateNowTime(int RID, int SID) {
    auto &r = data::robots[RID];
    auto &s = data::stations[SID];

    float rx = r.positionX, ry = r.positionY;
    float sx = s.positionX, sy = s.positionY;
    float distance = sqrt((sy-ry)*(sy-ry)+(sx-rx)*(sx-rx));
    float angle = atan2((sy-ry), (sx-rx));
    float alpha = angle - r.direction;
    if (alpha >  PAI)   alpha -= 2*PAI;
    if (alpha < -PAI)   alpha += 2*PAI;

    float time = 50*(distance/V_MAX+abs(alpha)/OMEGA_MAX);
    return time;
}