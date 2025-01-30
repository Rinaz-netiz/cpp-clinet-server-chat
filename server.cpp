#undef UNICODE

#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

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
    ClientSocket_1 = accept(ListenSocket, nullptr, nullptr);
    if (ClientSocket_1 == INVALID_SOCKET) {
        std::cout << "accept() failed with error: " << WSAGetLastError() << "\n";
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    std::cout << "Connection 1 established.\n";

    ClientSocket_2 = accept(ListenSocket, nullptr, nullptr);
    if (ClientSocket_2 == INVALID_SOCKET) {
        std::cout << "accept() failed with error: " << WSAGetLastError() << "\n";
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    std::cout << "Connection 2 established.\n";

    // No longer need server socket
    closesocket(ListenSocket);

    // Receive until the peer shuts down the connection
    std::cout << "Ready for chat\n";
    do {
        memset(recvbuf, 0, DEFAULT_BUFLEN);
        iResult = recv(ClientSocket_1, recvbuf, recvbuflen, 0);

        if(iResult<=0)  {
            std::cout << "recv() failed with error: " << WSAGetLastError() << "\n";
            break;
        }

        std::cout << recvbuf << "\n";

        // Echo the buffer back to the sender
        iSendResult = send( ClientSocket_2, recvbuf, iResult, 0 );
        if (iSendResult == SOCKET_ERROR) {
            std::cout << "send() failed with error: " << WSAGetLastError() << "\n";
            break;
        }
        std::cout << "Bytes sent: " << iSendResult << "\n";


        memset(recvbuf, 0, DEFAULT_BUFLEN);
        iResult = recv(ClientSocket_2, recvbuf, recvbuflen, 0);

        if(iResult<=0)  {
            std::cout << "recv() failed with error: " << WSAGetLastError() << "\n";
            break;
        }

        std::cout << recvbuf << "\n";

        // Echo the buffer back to the sender
        iSendResult = send( ClientSocket_1, recvbuf, iResult, 0 );
        if (iSendResult == SOCKET_ERROR) {
            std::cout << "send() failed with error: " << WSAGetLastError() << "\n";
            break;
        }
        std::cout << "Bytes sent: " << iSendResult << "\n";
    } while (true);

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