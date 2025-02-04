#undef UNICODE

#include <future>
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

void message(SOCKET* sock_r, SOCKET* sock_s) {
    int iResult=1;
    char recvbuf[DEFAULT_BUFLEN];
    while (iResult > 0) {


        memset(recvbuf, 0, DEFAULT_BUFLEN);
        iResult = recv(*sock_r, recvbuf, DEFAULT_BUFLEN, 0);
        if (iResult == SOCKET_ERROR) {
            std::cout << "recv failed. Error: " << WSAGetLastError() << "; disconect" << std::endl;
            *sock_r = INVALID_SOCKET;
            break;
        }

        iResult = send(*sock_s, recvbuf, DEFAULT_BUFLEN, 0);
        if (iResult == SOCKET_ERROR) {
            std::cout << "send failed. Error: " << WSAGetLastError() << "; disconect" << std::endl;
            *sock_s = INVALID_SOCKET;
            break;
        }
    }
}


int __cdecl main()
{
    WSADATA wsaData;
    int iResult;

    auto ListenSocket = INVALID_SOCKET;
    auto ClientSocket_1 = INVALID_SOCKET, ClientSocket_2 = INVALID_SOCKET;

    addrinfo *result = nullptr;
    addrinfo hints{};

    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

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

    // Accept a client socket

    //TODO сделать выход из цикла
    while(true) {
        //TODO выход по esc
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            std::cout << "shutdown server" << std::endl;
            break;
        }

        auto client = INVALID_SOCKET;
        client = accept(ListenSocket, nullptr, nullptr);
        if (ClientSocket_1 == INVALID_SOCKET) {
            ClientSocket_1 = client;
            std::cout << "connection established" << std::endl;
        }
        else if (ClientSocket_2 == INVALID_SOCKET) {
            ClientSocket_2 = client;
            std::cout << "connection established" << std::endl;
        }
        if(ClientSocket_1 != INVALID_SOCKET && ClientSocket_2 != INVALID_SOCKET) {
            std::cout << "chating in progress.." << std::endl;
            std::thread t1(message, &ClientSocket_1, &ClientSocket_2);
            t1.detach();
            std::thread t2(message, &ClientSocket_2, &ClientSocket_1);
            t2.detach();
        }
    }

    // TODO :: написать thread pool
    // TODO :: поменять на pthread
    // TODO :: мб помеянть логику перессылки(каждый раз подключаться к TP)


    closesocket(ListenSocket);

    // shutdown the connection since we're done
    iResult = shutdown(ClientSocket_1, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        std::cout << "shutdown() failed with error: " << WSAGetLastError() << "\n";
        closesocket(ClientSocket_1);
        WSACleanup();
        return 1;
    }
    iResult = shutdown(ClientSocket_2, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        std::cout << "shutdown() failed with error: " << WSAGetLastError() << "\n";
        closesocket(ClientSocket_2);
        WSACleanup();
        return 1;
    }

    // cleanup
    closesocket(ClientSocket_1);
    closesocket(ClientSocket_2);
    WSACleanup();


    return 0;
}