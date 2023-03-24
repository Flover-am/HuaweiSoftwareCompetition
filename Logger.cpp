#include <fstream>
#include <iostream>
#include "Logger.h"

Logger::Logger(bool is_append) {
    if (!is_use) {
        return;
    }
    this->logger_file = "zzz.in";
    if (!is_append) {
        out.open(this->logger_file);
        out << "Logger Init!" << endl;
        out.flush();
    }
}

void Logger::writeInfo(const string &in_message, bool mod) {
    if (level == "error"){
        return;
    }
    string message = "INFO: " + in_message;
    writeLog(message, mod);
}

void Logger::writeError(const string &in_message, bool mod) {
    string message = "ERROR:!!  " + in_message;
    writeLog(message, mod);
}

void Logger::writeLog(const string &message, bool mod) {
    if (!is_use){
        return;
    }
    out << message << endl;
    out.flush();

}
