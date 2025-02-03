#include <iostream>
#include <string>
#include <ostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>



// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512

void receive_message(SOCKET sock) {
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
        std::cout << recvbuf << std::endl;
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

    // Send a message
    std::cout << "Let's go!" << std::endl;
    std::thread t_r(receive_message, ConnectSocket);
    t_r.detach();
    while (true) {
        std::string message;
        // TODO :: сделать отключение от сервера !!
        getline(std::cin, message);
        if(message == "exit") {
            break;
        }
        iResult = send(ConnectSocket, message.c_str(), (int)message.length(), 0);
        if (iResult == SOCKET_ERROR) {
            std::cout << "Error sending message\n";
            break;
        }
    }

    std::cout << "disconect :/" << std::endl;
    // shutdown the connection since no more data will be sent
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        std::cout << "shutdown failed with error: " << WSAGetLastError() << std::endl;
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}