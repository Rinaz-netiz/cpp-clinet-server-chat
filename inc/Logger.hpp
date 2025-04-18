// Logger.h
#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <iostream>
#include <cstdarg>

class Logger {
public:
    static void info(const char* format, ...);
    static void error(const char* format, ...);
    static void fatal(const std::string& msg);
};

#endif
