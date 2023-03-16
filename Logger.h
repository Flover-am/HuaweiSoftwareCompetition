//
// Created by 18466 on 2023/3/16.
//

#ifndef CODECRAFTSDK_LOGGER_H
#define CODECRAFTSDK_LOGGER_H

#include <string>

using namespace std;

class Logger {
private:
    string logger_file;
public:
    Logger();
    void write_log(const string&, bool= false);
};


#endif //CODECRAFTSDK_LOGGER_H
