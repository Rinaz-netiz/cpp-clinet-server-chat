#include "utils.h"

#include <iostream>
#include <string>


void get_correct_password(std::string &password, const std::string& msg, const std::string &error_msg) {
    if (!msg.empty())
        std::cout << msg << std::endl;

    getline(std::cin, password);
    while (password.empty()) {
        std::cout << error_msg << std::endl;
        getline(std::cin, password);
    }
}
