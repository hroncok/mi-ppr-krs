#ifndef LOGGER_H
#define	LOGGER_H

#include <iostream>
#include <fstream>
#include <string>

//******************
// This class should be redesigned and reimplemented
// it is not possible to easily turn off debug/trace messages
//****************


/// Class for logging purposes
/// Implemented as Singleton design pattern
class Logger {
public:
    /// Get pointer to the instance of Logger
    /// Instance is lazy initialized (at the first call)
    static Logger* getInstance(int pID = 0, bool logToFile = true);

    /// Destructor, deletes allocated stream
    ~Logger();

    /// Overloaded <<
    template<typename T> Logger& operator<<(const T& object) {       
        //was not working well when the code was in .cpp file
        (*outStream) << object;
        if (logToFile) outStream->flush(); //making problems with cout (just try :))

        if (!outStream->good()) std::cerr << "Unspecified log error" << std::endl;
        return *this;
    }

private:
    /// Private constructor (it is Singleton)
    Logger(int pID, bool toFile);

    /// Empty, logger cannot be copied
    Logger(Logger const&) {
    } 

    /// Empty, logger cannot be assigned
    void operator=(Logger const&) {
    } 

    int pID; //rank of the processor
    std::ostream* outStream; // output stream (file or stdout)

private:
    bool logToFile; //logging into a file or stdout
    static Logger* instance;
};

#endif

