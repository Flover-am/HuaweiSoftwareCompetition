#include "main.h"
int main() {

    initMap();

    data::frame = 0;
    data::money = START_MONEY;
    vector<Step> defaultSteps;
    for (auto &path : data::pathTrees)
        for (auto &step : path)
            step = defaultSteps;
    for (auto &pathIndex : data::optedPaths)
        for (auto &stepIndex : pathIndex)
            stepIndex = -1;


    for (int frame = 1; frame <= FRAME_NUM; ++frame) {
        readMessage();  // 清空portRecvAskTime, 从输入加载portAvailableTime
        refresh();      // 从optedPath加载portRecvAskTime, 覆写portAvailableTime
        printf("%d\n", data::frame);
        logger.writeInfo("frame: "+to_string(data::frame) + "\n");

        for (int robotNum = 0; robotNum < ROBOT_NUM; ++robotNum)
            if (data::pathTrees[robotNum][STEP_DEPTH-1].empty())//  如果树不完全，准备规划路线
                setPathTree(robotNum);

        logger.writeInfo("here");

        for (int robotNum = 0; robotNum < ROBOT_NUM; ++robotNum)       //  选择一条树中的路线
            selectPath(robotNum);
        logger.writeInfo("here");

        outputNowPath();
        outputPathTree();

        for (int robotNum = 0; robotNum < ROBOT_NUM; ++robotNum) {
            int optIndex = data::optedPaths[robotNum][0];
            if (optIndex >= 0){
                Step &step = data::pathTrees[robotNum][0][optIndex];    //  如果有下一步规划，准备行动
                if (step.SID == data::robots[robotNum].stationID)  // 如果已经抵达目标,进行买卖命令
                    exchange(robotNum, step.SID, step.OID);
                else
                    navigate(robotNum, step.SID);

            }
        }
        puts("OK");
        fflush(stdout);
    }
    return 0;
}

void initMap() {
    int robot = 0, workStation = 0;
    string line;

    for (int i = 0; getline(cin, line); i++) {
        if (line == "OK")
            break;

        for (int j = 0; j < 100; ++j) {
            char symbol = line[j];
            if (symbol == '.')
                continue;
            float posX = TILE_SIZE*(j+0.5);
            float posY = TILE_SIZE*(TILE_NUM-i-0.5);
            if (symbol == 'A')
                data::robots.emplace_back(robot++, posX, posY);
            else
                data::stations.emplace_back(symbol-'0', workStation++, posX, posY);
        }
    }
    for (const auto &station : data::stations) {
        if (station.type == 4){
            data::receiveStationIDs[1].emplace_back(station.id);
            data::receiveStationIDs[2].emplace_back(station.id);
        }
        else if (station.type == 5){
            data::receiveStationIDs[1].emplace_back(station.id);
            data::receiveStationIDs[3].emplace_back(station.id);
        }
        else if (station.type == 6){
            data::receiveStationIDs[2].emplace_back(station.id);
            data::receiveStationIDs[3].emplace_back(station.id);
        }
        else if (station.type == 7){
            data::receiveStationIDs[4].emplace_back(station.id);
            data::receiveStationIDs[5].emplace_back(station.id);
            data::receiveStationIDs[6].emplace_back(station.id);
        }
        else if (station.type == 8){
            data::receiveStationIDs[7].emplace_back(station.id);
        }
        else if (station.type == 9){
            data::receiveStationIDs[1].emplace_back(station.id);
            data::receiveStationIDs[2].emplace_back(station.id);
            data::receiveStationIDs[3].emplace_back(station.id);
            data::receiveStationIDs[4].emplace_back(station.id);
            data::receiveStationIDs[5].emplace_back(station.id);
            data::receiveStationIDs[6].emplace_back(station.id);
            data::receiveStationIDs[7].emplace_back(station.id);
        }
    }
    for (auto &station1 : data::stations) {
        float time = 0, value;
        vector<float> tmpTime;
        vector<float> tmpValue;
        if (station1.type == 1){
            for (auto &station2 : data::stations) {
                time = calculateTime(station1.id, station2.id);
                if (station2.type == 4 || station2.type == 5)
                    value = calculateValue(station2.id, PROD_SELL_1);
                else if (station2.type == 9)
                    value = calculateValue(station2.id, ONLY_SELL);
                else
                    value = 0;
                tmpTime.emplace_back(time);
                tmpValue.emplace_back(value);
            }
        }
        else if (station1.type == 2){
            for (auto &station2 : data::stations) {
                time = calculateTime(station1.id, station2.id);
                if (station2.type == 4 || station2.type == 6)
                    value = calculateValue(station2.id, PROD_SELL_2);
                else if (station2.type == 9)
                    value = calculateValue(station2.id, ONLY_SELL);
                else
                    value = 0;
                tmpTime.emplace_back(time);
                tmpValue.emplace_back(value);
            }
        }
        else if (station1.type == 3){
            for (auto &station2 : data::stations) {
                time = calculateTime(station1.id, station2.id);
                if (station2.type == 5 || station2.type == 6)
                    value = calculateValue(station2.id, PROD_SELL_3);
                else if (station2.type == 9)
                    value = calculateValue(station2.id, ONLY_SELL);
                else
                    value = 0;
                tmpTime.emplace_back(time);
                tmpValue.emplace_back(value);
            }
        }
        else if (station1.type == 4){
            for (auto &station2 : data::stations) {
                time = calculateTime(station1.id, station2.id);
                if (station2.type == 7)
                    value = calculateValue(station2.id, PROD_SELL_4);
                else if (station2.type == 9)
                    value = calculateValue(station2.id, ONLY_SELL);
                else if (station2.type != 8)
                    value = calculateValue(station2.id, ONLY_BUY);
                else
                    value = 0;
                tmpTime.emplace_back(time);
                tmpValue.emplace_back(value);
            }
        }
        else if (station1.type == 5){
            for (auto &station2 : data::stations) {
                time = calculateTime(station1.id, station2.id);
                if (station2.type == 7)
                    value = calculateValue(station2.id, PROD_SELL_5);
                else if (station2.type == 9)
                    value = calculateValue(station2.id, ONLY_SELL);
                else if (station2.type != 8)
                    value = calculateValue(station2.id, ONLY_BUY);
                else
                    value = 0;
                tmpTime.emplace_back(time);
                tmpValue.emplace_back(value);
            }
        }
        else if (station1.type == 6){
            for (auto &station2 : data::stations) {
                time = calculateTime(station1.id, station2.id);
                if (station2.type == 7)
                    value = calculateValue(station2.id, PROD_SELL_6);
                else if (station2.type == 9)
                    value = calculateValue(station2.id, ONLY_SELL);
                else if (station2.type != 8)
                    value = calculateValue(station2.id, ONLY_BUY);
                else
                    value = 0;
                tmpTime.emplace_back(time);
                tmpValue.emplace_back(value);
            }
        }
        else if (station1.type == 7){
            for (auto &station2 : data::stations) {
                time = calculateTime(station1.id, station2.id);
                if (station2.type == 8 || station2.type == 9)
                    value = calculateValue(station2.id, ONLY_SELL);
                else
                    value = calculateValue(station2.id, ONLY_BUY);
                tmpTime.emplace_back(time);
                tmpValue.emplace_back(value);
            }
        }
        else{
            for (auto &station2 : data::stations) {
                time = calculateTime(station1.id, station2.id);
                if (station2.type != 8 && station2.type != 9)
                    value = calculateValue(station2.id, ONLY_BUY);
                else
                    value = 0;
                tmpTime.emplace_back(time);
                tmpValue.emplace_back(value);
            }
        }
        data::values.emplace_back(tmpValue);
    }
    puts("OK");
    fflush(stdout);
}
void readMessage() {
    string line;
    getline(cin, line);
    stringstream ss(line);
    ss >> data::frame >> data::money;
    getline(cin, line);

    int skipInt;    float skipFloat;
    for (auto &s : data::stations) {
        getline(cin, line);
        ss.clear();
        ss.str(line);

        int number, frameRemain, proState;
        array<int ,8> matState{};
        ss >> skipInt >> skipFloat >> skipFloat;
        ss >> frameRemain >> number >> proState;
        for (auto &state : matState){
            state = number%2;
            number /= 2;
        }
        // 清空condition, askRecved
        deque<Ask> emptyAsk;
        s.asksRecved = emptyAsk;
        deque<int> emptyPort;
        for (auto &frame : s.condition)
            frame.fill(emptyPort);

        // 读入condition, frameRemain
        for (int i = 1; i < 8; ++i)
            if (matState[i] == 1)
                for (auto &frame : s.condition)
                    frame[i].emplace_back(-1);
        // 初始化产品数量
        for (auto &frame : s.prosNum)
            frame = proState;
        for (int i = 0; i < UNAVAILABLE; ++i){
            s.frameRemain[i] = frameRemain-i > 0 ? frameRemain-i : 0;
            if (s.prosNum[i] == 0 && s.frameRemain[i] == 0){
                s.prosNum[i] = 1;
                s.frameRemain[i] = -1;
            }
        }
    }
    for (auto &r : data::robots) {
        getline(cin, line);
        ss.clear();
        ss.str(line);
        ss >> r.stationID >> r.item >> r.tValue >> r.hValue;
        ss >> r.angleV >> r.lineVX >> r.lineVY >> r.direction >> r.positionX >> r.positionY;
        if(r.item != 0){
            r.r = R_BIG;
            r.m = M_BIG;
            r.a = A_BIG;
            r.alpha = ALPHA_BIG;
        }
        else{
            r.r = R_SMALL;
            r.m = M_SMALL;
            r.a = A_SMALL;
            r.alpha = ALPHA_SMALL;
        }
    }
    getline(cin, line);
}
void refresh(){
    // 刷新权值与时间戳
    for (int robotNum = 0; robotNum < ROBOT_NUM; ++robotNum){
        for (auto &step : data::pathTrees[robotNum][0]){
            int SID = step.SID;
            float time = calculateNowTime(robotNum, SID);
            step.frame = (int)(time);
        }
    }
    // 从当前路径加载所有asksRecved
    for (int robotNum = 0; robotNum < ROBOT_NUM; ++robotNum){
        const auto &pathIndex = data::optedPaths[robotNum];
        if (pathIndex[STEP_DEPTH-1] == -1)
            continue;

        int targetIndex = pathIndex[STEP_DEPTH-1];
        float valueSum = data::pathTrees[robotNum][STEP_DEPTH-1][targetIndex].valueSum;

        for (int depth = 0; depth < STEP_DEPTH; ++depth){
            auto &stepIndex = pathIndex[depth];
            auto &step = data::pathTrees[robotNum][depth][stepIndex];
            if (step.OID == ONLY_SELL)  // 如果只卖，不会产生影响
                continue;

            auto &station = data::stations[step.SID];
            Ask ask{robotNum, step.OID, step.frameSum, valueSum};
            station.asksRecved.emplace_back(ask);
        }
    }

    // 根据asksRecved覆写condition
    for (auto &station : data::stations){
        // 将所有asksRecved按时间戳排序
        auto &asks = station.asksRecved;
        sort(asks.begin(), asks.end(), [](auto a, auto b){return a.timeStamp<b.timeStamp;});
        // 读入买请求
        for (int askID = 0; askID < asks.size(); ++askID){
            auto &ask = asks[askID];
            auto &condition = station.condition;
            if (ask.OID == ONLY_BUY)
                for (int i = ask.timeStamp; i < UNAVAILABLE; ++i)
                    condition[i][ask.OID].emplace_back(askID);
        }
        // 读取卖请求
        for (int askID = 0; askID < asks.size(); ++askID){
            auto &ask = asks[askID];
            auto &condition = station.condition;
            if (ask.OID == ONLY_BUY)
                continue;
            for (int i = ask.timeStamp; i < UNAVAILABLE; ++i)
                condition[i][ask.OID].emplace_back(askID);

            if (ask.OID < ONLY_SELL && ask.OID != ONLY_BUY){
                int k1, k2, k3;
                if (station.type == 4) {
                    k1 = 1; k2 = 2; k3 = -1;
                }
                else if (station.type == 5) {
                    k1 = 1; k2 = 3; k3 = -1;
                }
                else if (station.type == 6) {
                    k1 = 2; k2 = 3; k3 = -1;
                }
                else {
                    k1 = 4; k2 = 5; k3 = 6;
                }
                auto &Frame = condition[ask.timeStamp];
                int startProTime = ask.timeStamp;
                if ((!Frame[k1].empty() || ask.OID == k1) &&
                    (!Frame[k2].empty() || ask.OID == k2) &&
                    (k3 == -1 || !Frame[k3].empty() || ask.OID == k3)){
                    for (; startProTime < UNAVAILABLE; ++startProTime)
                        if (station.frameRemain[startProTime] == -1)
                            break;

                    for (int i = startProTime; i < UNAVAILABLE; ++i){
                        condition[i][k1].pop_front();
                        condition[i][k2].pop_front();
                        if (k3 != -1)
                            condition[i][k3].pop_front();
                        station.frameRemain[i] =
                                MEDIUM_TIME-(i-startProTime) > 0 ?
                                MEDIUM_TIME-(i-startProTime) : 0;
                    }
                    int finishProTime = startProTime+MEDIUM_TIME;
                    int prosNum = station.prosNum[finishProTime];
                    int ordersNum = station.condition[finishProTime][0].size();

                    if (prosNum <= ordersNum){
                        for (int i = finishProTime; i < UNAVAILABLE; ++i){
                            station.prosNum[i]++;
                            station.frameRemain[i] = 0;
                        }
                    }
                }
            }
        }
    }
}