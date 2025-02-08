#undef UNICODE

#include <future>
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#include "client_socket.h"

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

SOCKET create_socket();

void message(client_socket& sock_r, client_socket& sock_s) {
    int iResult=1;
    char recvbuf[DEFAULT_BUFLEN];
    while (iResult > 0) {
        memset(recvbuf, 0, DEFAULT_BUFLEN);
        iResult = recv(sock_r.client, recvbuf, DEFAULT_BUFLEN, 0);
        if (iResult == SOCKET_ERROR) {
            std::cout << "recv failed. Error: " << WSAGetLastError() << std::endl;
            sock_r.disconnect();
            break;
        }

        iResult = send(sock_s.client, recvbuf, DEFAULT_BUFLEN, 0);
        if (iResult == SOCKET_ERROR)
            break;
    }
}

void connection(client_socket& client_1, client_socket& client_2, SOCKET& server) {
    while (server != INVALID_SOCKET) {
        if (client_1.client == INVALID_SOCKET)
            client_1.connect(server);
        if (client_2.client == INVALID_SOCKET)
            client_2.connect(server);
        if (client_1.client != INVALID_SOCKET && client_2.client != INVALID_SOCKET) {
            std::thread t1(message, std::ref(client_1), std::ref(client_2));
            std::thread t2(message, std::ref(client_2), std::ref(client_1));
            std::cout << "Connection established." << std::endl;
            t1.join();
            t2.join();
        }

    }

}



int __cdecl main()
{
    SOCKET listen_sock = create_socket();

    client_socket client_1, client_2;
    // Accept a client socket
    std::thread connect(connection, std::ref(client_1), std::ref(client_2), std::ref(listen_sock));
    connect.detach();

    while(true) {
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            std::cout << "shutdown server" << std::endl;
            break;
        }
    }

    closesocket(listen_sock);

    WSACleanup();

    return 0;
}


SOCKET create_socket() {
    WSADATA wsaData;
    int iResult;

    auto ListenSocket = INVALID_SOCKET;

    addrinfo *result = nullptr;
    addrinfo hints{};

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(nullptr, DEFAULT_PORT, &hints, &result);
    if ( iResult != 0 ) {
        std::cout << "getaddrinfo failed with error: " << iResult << "\n";
        WSACleanup();
        return 1;
    }

    // Create a SOCKET for the server to listen for client connections.
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        std::cout << "socket() failed with error: " << WSAGetLastError() << "\n";
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket
    iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        std::cout << "bind() failed with error: " << WSAGetLastError() << "\n";
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        std::cout << "listen() failed with error: " << WSAGetLastError() << "\n";
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    std::cout << "Waiting for a connection...\n";
    return ListenSocket;
}
