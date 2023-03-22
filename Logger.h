#ifndef CODECRAFTSDK_LOGGER_H
#define CODECRAFTSDK_LOGGER_H

#include <string>

using namespace std;

class Logger {
private:
    string logger_file;

    void writeLog(const string &message, bool mod);

public:
    Logger(bool is_append);

    void writeInfo(const string &in_message, bool mod = false);
    void writeError(const string &in_message, bool mod);
};

#endif //CODECRAFTSDK_LOGGER_H
