#include <fstream>
#include <iostream>
#include "Logger.h"

Logger::Logger(bool is_append) {
    this->logger_file = "z_out.log";
    if (!is_append) {
        out.open(this->logger_file);
        out << "Logger Init!" << endl;
        out.flush();
    }
}

void Logger::writeInfo(const string &in_message, bool mod) {
    string message = "INFO: " + in_message;
    writeLog(message, mod);
}

void Logger::writeError(const string &in_message, bool mod) {
    string message = "ERROR:!!  " + in_message;
    writeLog(message, mod);
}

void Logger::writeLog(const string &message, bool mod) {

    out << message << endl;
    out.flush();
    
}
