#include "logger.h"
#include <sstream>

using namespace std; //I dont want to type it every time...

Logger* Logger::getInstance(int pID, bool toFile) {
    if (!instance) {
        instance = new Logger(pID, toFile);
    }
    return instance;
}

Logger::Logger(int pID, bool logToFile) {
    this->pID = pID;
    this->logToFile = logToFile; //logging to files or stdout

    if (logToFile) {
        // each proccess has own log-file
        // filenames: p1.log, p2.log, ... p(pCount-1).log
        stringstream ss;
        ss << "p" << pID << ".log";  //convert pID to string
        outStream = new ofstream(ss.str().c_str(), ios::out); //allocate new ofstream
        if (!outStream->good()) cerr << "Log-file cannot be open" << endl;
    } else {
        outStream = &cout;
    }
}


Logger::~Logger() {
    if (logToFile) {
        delete outStream;
    }
}

Logger* Logger::instance = NULL; //must be NULL before the first access