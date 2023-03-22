#ifndef CODECRAFTSDK_LOGGER_H
#define CODECRAFTSDK_LOGGER_H

#include <string>
#include <fstream>
using namespace std;

class Logger {
private:
    string logger_file;
    ofstream out;

    void writeLog(const string &message);

public:
    Logger();

    void writeInfo(const string &in_message);
    void writeError(const string &in_message);
    ~Logger();
};

#endif //CODECRAFTSDK_LOGGER_H
