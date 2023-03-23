#ifndef CODECRAFTSDK_LOGGER_H
#define CODECRAFTSDK_LOGGER_H

#include <string>
#include "fstream"
using namespace std;

class Logger {
private:
    string logger_file;

    void writeLog(const string &message, bool mod);

    ofstream out;
public:
    explicit Logger(bool is_append);

    void writeInfo(const string &in_message, bool mod = false);

    void writeError(const string &in_message, bool mod = false);
};

#endif //CODECRAFTSDK_LOGGER_H
