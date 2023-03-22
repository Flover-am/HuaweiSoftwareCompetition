#include "navigate.h"

void navigate(int RID, int SID){
    robot &r = data::robots[RID];
    station &s = data::stations[SID];
    double rx = r.positionX, ry = r.positionY;
    double sx = s.positionX, sy = s.positionY;
    double rv = sqrt(r.lineVX*r.lineVX+r.lineVY*r.lineVY);
    double angleV = r.angleV;

    double distance = sqrt((sy-ry)*(sy-ry)+(sx-rx)*(sx-rx));
    double angle = atan2((sy-ry), (sx-rx));
    double alpha = angle-r.direction;
    if (alpha >  PAI)   alpha -= 2*PAI;
    if (alpha < -PAI)   alpha += 2*PAI;

    double omega, v = -2; // 默认后退

    if (alpha > 0)  omega = OMEGA_MAX >  FRAMES_PER_S*alpha ? FRAMES_PER_S*alpha :  OMEGA_MAX;
    else            omega = OMEGA_MAX > -FRAMES_PER_S*alpha ? FRAMES_PER_S*alpha : -OMEGA_MAX;

    if (abs(alpha) <= PAI/2){
        double cosAlpha = cos(alpha), cosDelta = cos(alpha+omega*TIME_PER_FRAME);
        double tmp = (sqrt((1-cosAlpha*cosAlpha)/(1-cosDelta*cosDelta)));
        double maxV_ = distance*(cosAlpha-cosDelta*tmp); // 根据方程计算v的最大值

        if (!isnan(maxV_) && maxV_ < V_MAX) v = maxV_;
        else                                v = V_MAX;

        // 是否需要为碰撞半径留出空余？？
        if (r.direction > 0) {
            double maxV = (50-r.r-ry)*FRAMES_PER_S/sin(r.direction);
            if (v > maxV) v = maxV;
        }
        else if (r.direction < 0) {
            double maxV = (r.r-ry)*FRAMES_PER_S/sin(r.direction);
            if (v > maxV) v = maxV;
        }
        if (r.direction < PAI/2 && r.direction > -PAI/2) {
            double maxV = (50-r.r-rx)*FRAMES_PER_S/cos(r.direction);
            if (v > maxV) v = maxV;
        }
        else if (r.direction < -PAI/2 || r.direction > PAI/2) {
            double maxV = (r.r-rx)*FRAMES_PER_S/cos(r.direction);
            if (v > maxV) v = maxV;
        }
    }
    printf("rotate %d %f\n", RID, omega);
    printf("forward %d %f\n", RID, v);
    //TODO:没有计算一步运动所需的时间帧数，如可计算可用于路线规划中
    //TODO:倒行情况
}
double calculateTime(int RID, int SID, int OID, bool firstStep) {
    station &s = data::stations[SID];
    double distance, alpha = 0;
    double sx = s.positionX, sy = s.positionY;
    if (firstStep){
        auto &r = data::robots[RID];
        double rx = r.positionX, ry = r.positionY;
        distance = sqrt((sy-ry)*(sy-ry)+(sx-rx)*(sx-rx));
        alpha = atan2((sy-ry), (sx-rx))-r.direction;
        if (alpha >  PAI)   alpha -= 2*PAI;
        if (alpha < -PAI)   alpha += 2*PAI;
    }
    else{
        auto &S = data::stations[RID];
        double Sx = S.positionX, Sy = S.positionY;
        distance = sqrt((sy-Sy)*(sy-Sy)+(sx-Sx)*(sx-Sx));
    }

    double marchTime = 50*(distance/V_MAX+abs(alpha)/OMEGA_MAX);
    double awaitTime = 0;
    if (OID < ONLY_SELL)
        awaitTime = s.portAvailableTime[OID];

    double result = marchTime > awaitTime ? marchTime : awaitTime;
    return result;
}