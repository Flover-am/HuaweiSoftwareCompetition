#include "navigate.h"

void navigate(int RID, int SID){
    float rx = dataTable::robots[RID].positionX;
    float ry = dataTable::robots[RID].positionY;
    float sx = dataTable::workStations[SID].positionX;
    float sy = dataTable::workStations[SID].positionY;
    robot r = dataTable::robots[RID];

    float distance = sqrt((sy-ry)*(sy-ry)+(sx-rx)*(sx-rx));
    float angle = atan((sy-ry)/(sx-rx));
    if (sx < rx && sy < ry) angle -= PAI;
    if (sx < rx && sy > ry) angle += PAI;
    float alpha = r.direction-angle;
    if (alpha > PAI)        alpha -= 2*PAI;

    if (abs(alpha) < 0.005) //如果偏移角足够小，则不再旋转
        dataTable::needRotate[RID] = false;

    float omega = 0, v = V_MAX; //默认全速直线前进（已经计算过角度回正时如此前进不会错过目的地，但有待验证）
    if (dataTable::needRotate[RID]){
        if (alpha > 0)  omega = OMEGA_MAX >  FRAMES_PER_S*alpha ? FRAMES_PER_S*alpha :  OMEGA_MAX;
        else            omega = OMEGA_MAX > -FRAMES_PER_S*alpha ? FRAMES_PER_S*alpha : -OMEGA_MAX;

        if (abs(alpha) <= PAI/2){
            float cosAlpha = cos(alpha), cosDelta = cos(alpha+omega*TIME_PER_FRAME);
            float tmp = (sqrt((1-cosAlpha*cosAlpha)/(1-cosDelta*cosDelta)));
            float maxV_ = distance*(cosAlpha-cosDelta*tmp); // 根据方程计算v的最大值
            if (!isnan(maxV_) && v < V_MAX)  v = maxV_;
            //墙壁碰撞避免，角度回正时不需要进行
            float collideR = 0.45;  if (r.item != 0) collideR = 0.53; // 是否需要在此处留出空余量？
            if (r.direction > 0){
                float maxV = (50-collideR-ry)*FRAMES_PER_S/sin(r.direction);
                if (v > maxV) v = maxV;
            }
            else{
                float maxV = (collideR-ry)*FRAMES_PER_S/sin(r.direction);
                if (v > maxV) v = maxV;
            }
            if (r.direction < PAI/2 && r.direction > -PAI/2){
                float maxV = (50-collideR-rx)*FRAMES_PER_S/cos(r.direction);
                if (v > maxV) v = maxV;
            }
            else{
                float maxV = (collideR-rx)*FRAMES_PER_S/cos(r.direction);
                if (v > maxV) v = maxV;
            }
        }
    }
    printf("rotate %d %f\n", RID, omega);
    printf("forward %d %f\n", RID, v);
    //TODO:没有精确计算一步运动所需的时间帧数，如可计算可用于路线规划中；另外目前每帧都需要输出两条运动指令，也许可以优化
    //TODO:倒行情况
}