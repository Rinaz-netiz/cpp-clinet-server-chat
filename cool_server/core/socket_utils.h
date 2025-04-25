#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

class SocketUtils {
public:
    static SOCKET createSocket();
    static bool setReuseAddr(SOCKET socket);
    static bool bindSocket(SOCKET socket, int port);
    static bool startListening(SOCKET socket);
    static bool setNonBlocking(SOCKET socket);
    
    static std::string getLastErrorString();
};
