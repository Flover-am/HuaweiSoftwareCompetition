#ifndef CODECRAFTSDK_DATATABLE_H
#define CODECRAFTSDK_DATATABLE_H

#include "vector"
#include "robot.h"
#include "workStation.h"
using namespace std;

class dataTable {
public:
    int a;
    static int frame;
    static int money;
    static vector<robot> robots;
    static vector<workStation> workStations;

    dataTable(int);
};


#endif //CODECRAFTSDK_DATATABLE_H
