#ifndef UTILS_H
#define UTILS_H

#include <string>

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define BLUE    "\033[34m"
#define BG_YELLOW "\033[43m"
#define BOLD    "\033[1m"

void set_color_mode();
void get_correct_password(std::string &, const std::string &msg = "", const std::string& error_msg = "Your password is incorrect");

#endif //UTILS_H
