#include <iostream>
#include <string>
#include <ostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <vector>
#include <string.h>
#include <functional>

#include "des.h"


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512


void send_message(SOCKET sock, des::Des& des_client, std::vector<int> &key) {
    int sent_bytes=1;
    while (sent_bytes > 0) {
        std::string message;
        getline(std::cin, message);
        char* str_end = des_client.Encrypt(message.c_str(), key);
        sent_bytes = send(sock, str_end, (int)strlen(str_end), 0);
        // delete[] str_end;

        if (sent_bytes == SOCKET_ERROR) {
            std::cout << "Error sending message\n";
            break;
        }
    }
}

void receive_message(SOCKET sock, des::Des& des_client, std::vector<int> &key) {
    int received_bytes=1;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
    while (received_bytes > 0) {
        memset(recvbuf, 0, recvbuflen);
        received_bytes = recv(sock, recvbuf, recvbuflen, 0);
        if (received_bytes == SOCKET_ERROR) {
            std::cout << "Error receiving message\n";
            break;
        }

        const char* str_dec = des_client.Decrypt(recvbuf, key);
        std::cout << str_dec << std::endl;
        delete[] str_dec;
    }
}

int __cdecl main()
{
    WSADATA wsaData;
    auto ConnectSocket = INVALID_SOCKET;
    addrinfo *result = nullptr,
                    *ptr = nullptr,
                    hints{};

    int iResult;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        std::cout << "WSAStartup failed with error: " << iResult << std::endl;
        return 1;
    }

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(nullptr, DEFAULT_PORT, &hints, &result);
    if ( iResult != 0 ) {
        std::cout << "getaddrinfo failed with error: " << iResult << std::endl;
        WSACleanup();
        return 1;
    }

    // Attempt to connect to an address until one succeeds
    for(ptr=result; ptr != nullptr ;ptr=ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            std::cout << "socket failed with error: " << WSAGetLastError() << std::endl;
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
        std::cout << "unable to connect to server" << std::endl;
        WSACleanup();
        return 1;
    }

    // get password
    std::vector<int> key(KEY_LEN);
    std::string password;
    std::cout << "Enter your key(8 characters): ";
    getline(std::cin, password);
    des::key_to_binary(key, password);

    // create Des
    des::Des des_client;

    // Send a message
    std::cout << "Let's go!" << std::endl;
    std::thread t_r(receive_message, ConnectSocket, std::ref(des_client), std::ref(key));
    std::thread t_s(send_message, ConnectSocket, std::ref(des_client), std::ref(key));

    t_r.join();
    t_s.join();

    // shutdown the connection since no more data will be sent
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        std::cout << "shutdown failed with error: " << WSAGetLastError() << std::endl;
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }
    std::cout << "disconect :/" << std::endl;

    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}