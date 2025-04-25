#include "socket_utils.h"
#include <iostream>

SOCKET SocketUtils::createSocket() {
    SOCKET sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Socket creation failed: " << getLastErrorString() << "\n";
    }
    return sock;
}

bool SocketUtils::setReuseAddr(SOCKET socket) {
    int yes = 1;
    if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (char*)&yes, sizeof(yes)) == SOCKET_ERROR) {
        std::cerr << "Setsockopt failed: " << getLastErrorString() << "\n";
        return false;
    }
    return true;
}

bool SocketUtils::bindSocket(SOCKET socket, int port) {
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(socket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed: " << getLastErrorString() << "\n";
        return false;
    }
    return true;
}

bool SocketUtils::startListening(SOCKET socket) {
    if (listen(socket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed: " << getLastErrorString() << "\n";
        return false;
    }
    return true;
}

bool SocketUtils::setNonBlocking(SOCKET socket) {
    u_long mode = 1;
    if (ioctlsocket(socket, FIONBIO, &mode) == SOCKET_ERROR) {
        std::cerr << "Non-blocking mode failed: " << getLastErrorString() << "\n";
        return false;
    }
    return true;
}

std::string SocketUtils::getLastErrorString() {
    DWORD error = WSAGetLastError();
    LPSTR messageBuffer = nullptr;
    size_t size = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&messageBuffer, 0, NULL);

    std::string message(messageBuffer, size);
    LocalFree(messageBuffer);
    return message;
}
