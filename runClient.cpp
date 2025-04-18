#include "inc/Client.hpp"
#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <Name>" << std::endl;
        return 1;
    }

    ChatClient client(argv[1], "127.0.0.1", 5208);
    client.run();

    return 0;
}