//
// Created by 18466 on 2023/3/16.
//

#ifndef CODECRAFTSDK_DATATABLE_H
#define CODECRAFTSDK_DATATABLE_H

#include "vector"

using namespace std;

class dataTable {
public:
    // 帧数
    int a;
    static int frame;
    // 钱数
    static int money;
    static vector<robot> robots;
    static vector<workStation> workStations;

    dataTable(int);
};


#endif //CODECRAFTSDK_DATATABLE_H
