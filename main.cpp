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

        readMessage();
        refresh(); // 刷新station.portAvailableTime
        printf("%d\n", data::frame);

        for (auto &station : data::stations){
            string str = to_string(station.id) + "\n";
            for (int o = 0; o < 8; ++o){
                for (auto &ask : station.portRecvAskTime[o]){
                    str += "port" + to_string(o) + ": " + to_string(ask.RID) + " "
                            + to_string(ask.timeStamp) + " "
                            + to_string(ask.value) + "\n";
                }
            }
            logger.writeInfo(str,false);
        }

        for (int robotNum = 0; robotNum < ROBOT_NUM; ++robotNum)
            if (data::pathTrees[robotNum][STEP_DEPTH-1].empty())      //  如果树不完全，准备规划路线
                setPathTree(robotNum);

        for (int robotNum = 0; robotNum < ROBOT_NUM; ++robotNum)       //  选择一条树中的路线
            selectPath(robotNum);

        int p = 0;
        for (auto &pathIndex : data::optedPaths){
            string s;
            int x = 0;
            for (auto &stepIndex : pathIndex){
                auto steps = data::pathTrees[p][x];
                s += to_string(steps[stepIndex].SID) + " ";
                ++x;
            }
            logger.writeInfo(s, false);
            ++p;
        }

        for (int robotNum = 0; robotNum < ROBOT_NUM; ++robotNum){
            int optIndex = data::optedPaths[robotNum][0];
            if (optIndex != -1){
                Step &step = data::pathTrees[robotNum][0][optIndex];    //  如果有下一步规划，准备行动
                if (step.SID == data::robots[robotNum].stationID)       // 如果已经抵达目标,进行买卖命令
                    exchange(robotNum, step.SID, step.OID);
                if (step.SID != data::robots[robotNum].stationID)       // 如果有Destination且尚未抵达,进行移动命令
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

            double posX = (j+0.5)*TILE_SIZE, posY = (TILE_NUM-i-0.5)*TILE_SIZE;
            if (symbol == 'A'){
                data::robots.emplace_back(robot++, posX, posY);
                if (robot > ROBOT_NUM)
                    logger.writeError("Robot more than 4.", true);
            }
            else if (isdigit(symbol))
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
    puts("OK");
    fflush(stdout);
}
void readMessage() {
    string line;
    getline(cin, line);
    stringstream ss(line);
    ss >> data::frame >> data::money;
    getline(cin, line);

    int skipInt;    double skipFloat;
    for (auto &s : data::stations) {
        getline(cin, line);
        ss.clear();
        ss.str(line);

        int number, proState;
        array<int ,8> matState{};
        ss >> skipInt >> skipFloat >> skipFloat;
        ss >> s.timeRemain >> number >> s.proState;
        for (auto &state : s.matState){
            state = number%2;
            number /= 2;
        }
        // 清空recvAskTime，等待本轮加载
        vector<Ask> emptyAsk;
        s.portRecvAskTime.fill(emptyAsk);
        // 读入portAvailableTime[0]
        for (int i = 1; i < 8; i++)
            s.portAvailableTime[i] = -1;
        if (s.proState == 1)
            s.portAvailableTime[0] = 0;       // 当前可用
        else if (s.timeRemain != -1){
            s.portAvailableTime[0] = s.timeRemain;

        }
        else
            s.portAvailableTime[0] = -1;
    }
    // 读入portAvailableTime[1-7]
    for (int i = 1; i < 8; i++)
        for (auto &SID : data::receiveStationIDs[i]){
            auto &s = data::stations[SID];
            if (s.matState[i] == 0)   // 当前可用
                s.portAvailableTime[i] = 0;
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
    // TODO: 可用时间帧数组
    // 从当前路径加载所有receiveAskTime
    for (int robotNum = 0; robotNum < ROBOT_NUM; ++robotNum){
        const auto &pathIndex = data::optedPaths[robotNum];
        if (pathIndex[STEP_DEPTH-1] == -1)
            continue;

        for (int depth = 0; depth < STEP_DEPTH; ++depth){
            auto &stepIndex = pathIndex[depth];
            auto &step = data::pathTrees[robotNum][depth][stepIndex];
            if (step.OID == ONLY_SELL)  // 如果只卖，不会产生影响
                continue;

            auto &station = data::stations[step.SID];
            Ask ask{robotNum, step.frameSum, step.valueSum};
            station.portRecvAskTime[step.OID].emplace_back(ask);
        }
    }
    // 根据receiveAskTime覆写portAvailableTime
    for (auto &station : data::stations){
        int recvTime = data::frame;
        auto &ports = station.portAvailableTime;
        auto &asks = station.portRecvAskTime;

        if (station.type < 4){}
        else if (station.type == 4){
            if ((ports[1] == -1 || !asks[1].empty()) &&
                (ports[2] == -1 || !asks[2].empty())){
                if (!asks[1].empty())
                    recvTime = asks[1][0].timeStamp > recvTime?
                               asks[1][0].timeStamp : recvTime;
                if (!asks[2].empty())
                    recvTime = asks[2][0].timeStamp > recvTime?
                               asks[2][0].timeStamp : recvTime;
                if (ports[0] == -1){        //如果没有被阻塞
                    ports[1] = recvTime;
                    ports[2] = recvTime;
                    ports[0] = recvTime+MEDIUM_TIME;
                }
            }
        }
        else if (station.type == 5){
            if ((ports[1] == -1 || !asks[1].empty()) &&
                (ports[3] == -1 || !asks[3].empty())){
                if (!asks[1].empty())
                    recvTime = asks[1][0].timeStamp > recvTime?
                               asks[1][0].timeStamp : recvTime;
                if (!asks[3].empty())
                    recvTime = asks[3][0].timeStamp > recvTime?
                               asks[3][0].timeStamp : recvTime;
                if (ports[0] == -1){        //如果没有被阻塞
                    ports[1] = recvTime;
                    ports[3] = recvTime;
                    ports[0] = recvTime+MEDIUM_TIME;
                }
            }
        }
        else if (station.type == 6){
            if ((ports[2] == -1 || !asks[2].empty()) &&
                (ports[3] == -1 || !asks[3].empty())){
                if (!asks[2].empty())
                    recvTime = asks[2][0].timeStamp > recvTime?
                               asks[2][0].timeStamp : recvTime;
                if (!asks[3].empty())
                    recvTime = asks[3][0].timeStamp > recvTime?
                               asks[3][0].timeStamp : recvTime;
                if (ports[0] == -1){        //如果没有被阻塞
                    ports[2] = recvTime;
                    ports[3] = recvTime;
                    ports[0] = recvTime+MEDIUM_TIME;
                }
            }
        }
        else if (station.type == 7){
            if ((ports[4] == -1 || !asks[4].empty()) &&
                (ports[5] == -1 || !asks[5].empty()) &&
                (ports[6] == -1 || !asks[6].empty())){
                if (!asks[4].empty())
                    recvTime = asks[4][0].timeStamp > recvTime?
                               asks[4][0].timeStamp : recvTime;
                if (!asks[5].empty())
                    recvTime = asks[5][0].timeStamp > recvTime?
                               asks[5][0].timeStamp : recvTime;
                if (!asks[6].empty())
                    recvTime = asks[6][0].timeStamp > recvTime?
                               asks[6][0].timeStamp : recvTime;
                if (ports[0] == -1){        //如果没有被阻塞
                    ports[4] = recvTime;
                    ports[5] = recvTime;
                    ports[6] = recvTime;
                    ports[0] = recvTime+LONG_TIME;
                }
            }
        }
    }
}

