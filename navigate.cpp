#include "navigate.h"
#include "avoidCollision.h"

void navigate(int RID, int SID, bool no) {

    if (no) {
        printf("rotate %d 0\n", RID);
        printf("forward %d 0\n", RID);
        data::have_printed[RID] = true;
        logger.writeInfo("print" + to_string(RID) + " _A");
    }
    if (data::frame > 500 && data::frame < 600) {
        logger.writeInfo("");
        logger.writeInfo("frame:=" + to_string(data::frame));
    }
    if (!avoidCollision::judgeColl(RID) && !data::have_printed[RID]) {
        robot &r = data::robots[RID];
        workStation &s = data::workStations[SID];
        float rx = r.positionX, ry = r.positionY;
        float sx = s.positionX, sy = s.positionY;
        float rv = sqrt(r.lineVX * r.lineVX + r.lineVY * r.lineVY), angleV = r.angleV;

        float distance = sqrt((sy - ry) * (sy - ry) + (sx - rx) * (sx - rx));
        float angle = atan2((sy - ry), (sx - rx));
        float alpha = angle - r.direction;
        if (alpha > PAI) alpha -= 2 * PAI;
        if (alpha < -PAI) alpha += 2 * PAI;
        float omega, v = -2; // 默认后退

        if (alpha > 0) omega = OMEGA_MAX > FRAMES_PER_S * alpha ? FRAMES_PER_S * alpha : OMEGA_MAX;
        else omega = OMEGA_MAX > -FRAMES_PER_S * alpha ? FRAMES_PER_S * alpha : -OMEGA_MAX;

        if (abs(alpha) <= PAI / 2) {
            float cosAlpha = cos(alpha), cosDelta = cos(alpha + omega * TIME_PER_FRAME);
            float tmp = (sqrt((1 - cosAlpha * cosAlpha) / (1 - cosDelta * cosDelta)));
            float maxV_ = distance * (cosAlpha - cosDelta * tmp); // 根据方程计算v的最大值

            if (!isnan(maxV_) && maxV_ < V_MAX) v = maxV_;
            else v = V_MAX;

            // 是否需要为碰撞半径留出空余？？
            if (r.direction > 0) {
                float maxV = (50 - r.r - ry) * FRAMES_PER_S / sin(r.direction);
                if (v > maxV) v = maxV;
            } else if (r.direction < 0) {
                float maxV = (r.r - ry) * FRAMES_PER_S / sin(r.direction);
                if (v > maxV) v = maxV;
            }
            if (r.direction < PAI / 2 && r.direction > -PAI / 2) {
                float maxV = (50 - r.r - rx) * FRAMES_PER_S / cos(r.direction);
                if (v > maxV) v = maxV;
            } else if (r.direction < -PAI / 2 || r.direction > PAI / 2) {
                float maxV = (r.r - rx) * FRAMES_PER_S / cos(r.direction);
                if (v > maxV) v = maxV;
            }
        }
        printf("rotate %d %f\n", RID, omega);
        printf("forward %d %f\n", RID, v);
        data::have_printed[RID] = true;
        logger.writeInfo("print" + to_string(RID) + " _A");
    }
    //TODO:没有计算一步运动所需的时间帧数，如可计算可用于路线规划中
    //TODO:倒行情况
}

float calculateTime(int RID, pair<int, int> step, bool firstStep) {
    workStation &s = data::workStations[step.first];
    float distance, alpha = 0;
    float sx = s.positionX, sy = s.positionY;
    if (firstStep) {
        auto &r = data::robots[RID];
        float rx = r.positionX, ry = r.positionY;
        distance = sqrt((sy - ry) * (sy - ry) + (sx - rx) * (sx - rx));
        alpha = atan2((sy - ry), (sx - rx)) - r.direction;
        if (alpha > PAI) alpha -= 2 * PAI;
        if (alpha < -PAI) alpha += 2 * PAI;
    } else {
        auto &S = data::workStations[RID];
        float Sx = S.positionX, Sy = S.positionY;
        distance = sqrt((sy - Sy) * (sy - Sy) + (sx - Sx) * (sx - Sx));
    }
    // TODO: 目前采用简单计算距离的方法，算法待优化，还需要考虑物品售价以及行进时间与目标工作台工作剩余时间
    float marchTime = 50 * (distance / V_MAX + abs(alpha) / OMEGA_MAX);
    float awaitTime = 0;
    if (step.second < ONLY_SELL)
        if (step.second == ONLY_BUY || s.matState[step.second] == 1)
            awaitTime = (float) (s.timeRemain);
    float result = marchTime > awaitTime ? marchTime : awaitTime;
    return result;
}