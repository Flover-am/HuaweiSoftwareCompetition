#include "main.h"

int main() {

    initMap();

    data::frame = 1;
    data::money = START_MONEY;
    vector<Step> defaultSteps;
    for (auto &path : data::pathTrees)
        for (auto &step : path)
            step = defaultSteps;
    for (auto &num : data::pathNums)
        num = -1;

    for (int frame = 0; frame < FRAME_NUM; ++frame) {
        readMessage();
        printf("%d\n", data::frame++);


        for (int robotNum = 0; robotNum < ROBOT_NUM; ++robotNum){
            selectPath(robotNum);
        }
        /*for (int robotNum = 0; robotNum < ROBOT_NUM; ++robotNum)
            // 如果需要购买，检测之后是否存在新的出售冲突
            if (ONLY_BUY == data::destList[robotNum][STEP_DEPTH-1].second){
                auto SID = data::destList[robotNum][STEP_DEPTH-1].first;
                detectConflict(robotNum, data::stations[SID].type);
            }*/

        for (int robotNum = 0; robotNum < ROBOT_NUM; ++robotNum)
            if (data::pathTrees[robotNum][STEP_DEPTH - 1].empty())      //  如果规划不完全，准备规划路线
                setPathTree(robotNum);

        for (int robotNum = 0; robotNum < ROBOT_NUM; ++robotNum){
            auto &steps = data::pathTrees[robotNum][0];
            if (!steps.empty()){                                        //  如果有下一步规划，准备行动
                if (steps[0].SID == data::robots[robotNum].stationID)   // 如果已经抵达目标,进行买卖命令
                    exchange(robotNum, steps[0].SID, steps[0].OID);
                if (steps[0].SID != data::robots[robotNum].stationID)   // 如果有Destination且尚未抵达,进行移动命令
                    navigate(robotNum, steps[0].SID);
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

            float posX = (j+0.5)*TILE_SIZE, posY = (TILE_NUM-i-0.5)*TILE_SIZE;
            if (symbol == 'A'){
                data::robots.emplace_back(robot++, posX, posY);
                if (robot > ROBOT_NUM)
                    logger.writeError("Robot more than 4.", true);
            }
            else if (isdigit(symbol))
                data::stations.emplace_back(symbol - '0', workStation++, posX, posY);
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

    int skipInt = 0;    float skipFloat = 0;
    for (auto &s : data::stations) {
        int number, timeRemain, proState;
        array<int, 8> matState{};
        getline(cin, line);
        ss.clear();
        ss.str(line);
        ss >> skipInt >> skipFloat >> skipFloat;
        ss >> timeRemain >> number >> proState;

        for (auto &state : matState){
            state = number%2;
            number /= 2;
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

