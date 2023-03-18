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

    void write_log(const string &, bool= false);

public:
    Logger(bool is_append);

    void write_info(const string &, bool= false);

    void write_error(const string &, bool= false);

};


#endif //CODECRAFTSDK_LOGGER_H
