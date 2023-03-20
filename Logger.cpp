#include <fstream>
#include <iostream>
#include "Logger.h"

Logger::Logger(bool is_append) {
    this->logger_file = "z_out.log";
    if (!is_append) {
        ofstream out(this->logger_file);
        if (!out) {
            cerr << "file not open" << endl;
        } else {
            out << "Logger Init!" << endl;
            out.flush();
            out.close();
        }
    } else {
        ofstream out(this->logger_file, ios::app);
        if (!out) {
            cerr << "file not open" << endl;
        } else {
            out << "New User Open the Logger" << endl;
            out.flush();
            out.close();
        }
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
    if (mod) {
        ofstream out(this->logger_file, ios::out);
        if (!out) {
        } else {
            out << message << endl;
            out.flush();
            out.close();
        }
    } else {
        ofstream out(this->logger_file, ios::app);
        if (!out) {
        } else {
            out << message << endl;
            out.flush();
            out.close();
        }
    }
}
