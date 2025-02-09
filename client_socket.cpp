#include "client_socket.h"
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

void client_socket::connect(SOCKET l_sock) {
    client_socket::client = accept(l_sock, nullptr, nullptr);
    std::cout << client_socket::client << std::endl;

}

void client_socket::disconnect(){
    closesocket(client_socket::client);
    client_socket::client = INVALID_SOCKET;
    std::cout << "client is disconnected " << std::endl;
}
