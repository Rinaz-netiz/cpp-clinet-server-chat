#include <iostream>
#include <string>
#include <ostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <vector>
#include <cstring>
#include <functional>

#include "des.h"
#include "logger.h"


//todo переделать клиентв


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512

SOCKET create_socket(Logger&);

void send_message(Logger& logger, SOCKET sock, Des& des_client, std::vector<int> &key) {
    int iResult = 1;
    while (true) {
        std::string message;
        getline(std::cin, message);

        if(message == "exit")
            message = "**Your interlocutor has left.**";

        char* str_end = des_client.Encrypt(message.c_str(), key);

        iResult = send(sock, str_end, (int)strlen(str_end), 0);

        if(message == "exit") {
            logger.log(LogLevel::INFO, "You have gone out from session");
            break;
        }

        if (iResult == SOCKET_ERROR) {
            logger.log(LogLevel::ERROR_, "Error sending message\n");
            break;
        }
    }
}

int __cdecl main()
{
    Logger& logger = Logger::getInstance();
    logger.setLogLevel(LogLevel::INFO);

    int iResult;
    SOCKET ConnectSocket = create_socket(logger);
  
      // get password
    std::vector<int> key(KEY_LEN);
    std::string password;
    std::cout << "Enter your key(8 characters or more): ";
    getline(std::cin, password);
    key_to_binary(key, password);

    // create Des
    Des des_client;

    // Send a message
    // std::cout << "Let's go!" << std::endl;
    logger.log(LogLevel::INFO, "Client is running");

    std::cout << "write 'exit' to leave" << std::endl;
    std::thread t_r(send_message, std::ref(logger), ConnectSocket, std::ref(des_client), std::ref(key));
    t_r.detach();

    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
    iResult=1;
    while (iResult > 0) {
        memset(recvbuf, 0, recvbuflen);
        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        if (iResult == SOCKET_ERROR) {
            logger.log(LogLevel::ERROR_, "Error receiving message");
            break;
        }

        const char* str_dec = des_client.Decrypt(recvbuf, key);
        std::cout << str_dec << std::endl;
        delete[] str_dec;
    }


    logger.log(LogLevel::INFO, "disconect :");
    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}

SOCKET create_socket(Logger& logger) {
    WSADATA wsaData;
    auto ConnectSocket = INVALID_SOCKET;
    addrinfo *result = nullptr,
                    *ptr = nullptr,
                    hints{};

    int iResult;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        std::string errorMessage = "WSAStartup failed with error: " + std::to_string(iResult);
        logger.log(LogLevel::ERROR_, errorMessage);
        return 1;
    }

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(nullptr, DEFAULT_PORT, &hints, &result);
    if ( iResult != 0 ) {
        std::string errorMessage = "getaddrinfo failed with error: " + std::to_string(iResult);
        logger.log(LogLevel::ERROR_, errorMessage);
        WSACleanup();
        return 1;
    }

    // Attempt to connect to an address until one succeeds
    for(ptr=result; ptr != nullptr ;ptr=ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            std::string errorMessage = "socket failed with error: " + std::to_string(WSAGetLastError());
            logger.log(LogLevel::ERROR_, errorMessage);
            WSACleanup();
            return 1;
        }

        // Connect to server.
        iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        logger.log(LogLevel::ERROR_, "unable to connect to server");
        WSACleanup();
        return INVALID_SOCKET;
    }
    return ConnectSocket;
}
