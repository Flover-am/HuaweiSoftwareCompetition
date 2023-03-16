//
// Created by 18466 on 2023/3/16.
//

#include <fstream>
#include "Logger.h"
#include <iostream>

Logger::Logger() {
    this->logger_file = "z_out.log";
    ofstream out(this->logger_file);
    if (!out) {
//        cerr << "file not open" << endl;
    } else {
        out.flush();
        out.close();
    }
}

void Logger::write_log(const string& message, bool mod) {
    if (mod) {
        ofstream out(this->logger_file, ios::app);
        if (!out) {
//            cerr << "file not open" << endl;
        } else {
            out << message << endl;
            out.flush();
            out.close();
        }
    } else {
        ofstream out(this->logger_file, ios::out);
        if (!out) {
//            cerr << "file not open" << endl;
        } else {
            out << message << endl;
            out.flush();
            out.close();
        }
    }
}
