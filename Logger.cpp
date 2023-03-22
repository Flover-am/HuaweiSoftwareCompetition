#include <iostream>
#include "Logger.h"

Logger::Logger() {
    this->logger_file = "z_out.log";
    if (!out.is_open()) {
        out.open(this->logger_file);
        out << "Logger Init!" << endl;
        if (!out) {
            cerr << "file not open" << endl;
        }
    }

}


void Logger::writeInfo(const string &message) {
    writeLog(message);
}

void Logger::writeError(const string &in_message) {
    string message = "ERROR:!!  " + in_message;
    writeLog(message);
}

void Logger::writeLog(const string &message) {
    if (out) {
        out << message << endl;
        out.flush();
    }
}

Logger::~Logger() {
    out.close();
}
