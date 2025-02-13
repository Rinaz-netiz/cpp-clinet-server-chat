#undef UNICODE

#include <future>
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>

#include "client_socket.h"
#include "logger/logger.h"

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

SOCKET create_socket(Logger&);

void message(Logger& logger, client_socket& sock_r, client_socket& sock_s) {
    int iResult=1;
    char recvbuf[DEFAULT_BUFLEN];
    while (iResult > 0) {
        memset(recvbuf, 0, DEFAULT_BUFLEN);
        iResult = recv(sock_r.client, recvbuf, DEFAULT_BUFLEN, 0);
        if (iResult == SOCKET_ERROR) {
            std::string error_msg = "recv failed. Error: " + std::to_string(WSAGetLastError());
            logger.log(LogLevel::ERROR_, error_msg);
            sock_r.disconnect();
            break;
        }

        iResult = send(sock_s.client, recvbuf, DEFAULT_BUFLEN, 0);
        if (iResult == SOCKET_ERROR)
            break;
    }
}

void connection(Logger& logger, client_socket& client_1, client_socket& client_2, SOCKET& server) {
    while (server != INVALID_SOCKET) {
        if (client_1.client == INVALID_SOCKET)
            client_1.connect(server);
        if (client_2.client == INVALID_SOCKET)
            client_2.connect(server);
        if (client_1.client != INVALID_SOCKET && client_2.client != INVALID_SOCKET) {
            std::thread t1(message, std::ref(logger), std::ref(client_1), std::ref(client_2));
            std::thread t2(message, std::ref(logger), std::ref(client_2), std::ref(client_1));
            logger.log(LogLevel::INFO, "Connection established.");
            t1.join();
            t2.join();
        }

    }

}



int __cdecl main()
{
    Logger& logger = Logger::getInstance();
    SOCKET listen_sock = create_socket(logger);

    client_socket client_1, client_2;
    // Accept a client socket
    std::thread connect(connection, std::ref(logger), std::ref(client_1), std::ref(client_2), std::ref(listen_sock));
    connect.detach();

    while(true) {
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            logger.log(LogLevel::INFO, "shutdown server");
            break;
        }
    }

    closesocket(listen_sock);

    WSACleanup();

    return 0;
}


SOCKET create_socket(Logger& logger) {
    WSADATA wsaData;
    int iResult;

    auto ListenSocket = INVALID_SOCKET;

    addrinfo *result = nullptr;
    addrinfo hints{};

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        std::string error_msg = "WSAStartup failed with error: " + std::to_string(iResult);
        logger.log(LogLevel::ERROR_, error_msg);
        return 1;
    }

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(nullptr, DEFAULT_PORT, &hints, &result);
    if ( iResult != 0 ) {
        std::string error_msg = "getaddrinfo failed with error: " + std::to_string(iResult);
        logger.log(LogLevel::ERROR_, error_msg);
        WSACleanup();
        return 1;
    }

    // Create a SOCKET for the server to listen for client connections.
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        std::string error_msg = "socket() failed with error: " + std::to_string(WSAGetLastError());
        logger.log(LogLevel::ERROR_, error_msg);
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket
    iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        std::string error_msg = "bind() failed with error: " + std::to_string(WSAGetLastError());
        logger.log(LogLevel::ERROR_, error_msg);
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        std::string error_msg = "listen() failed with error: " + std::to_string(WSAGetLastError());
        logger.log(LogLevel::ERROR_, error_msg);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    logger.log(LogLevel::INFO, "Waiting for a connection...");
    return ListenSocket;
}
