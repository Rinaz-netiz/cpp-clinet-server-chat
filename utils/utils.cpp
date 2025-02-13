#include "utils.h"

#include <iostream>
#include <string>
#include <windows.h>

#include "../logger/logger.h"

void set_color_mode() {
    Logger& logger = Logger::getInstance();

    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) {
        logger.log(LogLevel::ERROR_, "Error getting console handle");
        return;
    }

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) {
        logger.log(LogLevel::ERROR_, "Error getting console mode");
        return;
    }

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(hOut, dwMode)) {
        logger.log(LogLevel::ERROR_, "Error setting console mode");
        return;
    }

    logger.log(LogLevel::INFO, "Console color mode set successfully");
}


void get_correct_password(std::string &password, const std::string& msg, const std::string &error_msg) {
    if (!msg.empty())
        std::cout << msg << std::endl;

    getline(std::cin, password);
    while (password.empty()) {
        std::cout << RED << error_msg << RESET << std::endl;
        getline(std::cin, password);
    }
}
