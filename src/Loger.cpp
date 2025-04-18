// Logger.cpp
#include "../inc/Logger.hpp"
#include <cstdio>
#include <cstdlib>

void Logger::info(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
}

void Logger::error(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
}

void Logger::fatal(const std::string& msg) {
    std::cerr << msg << std::endl;
    exit(1);
}
