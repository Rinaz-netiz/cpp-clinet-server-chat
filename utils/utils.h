#ifndef UTILS_H
#define UTILS_H

#include<string>

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"

void get_correct_password(std::string &, const std::string &msg = "", const std::string& error_msg = "Your password is incorrect");
void set_color_mode();

#endif //UTILS_H
