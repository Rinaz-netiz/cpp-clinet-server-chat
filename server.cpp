/*
Написать клиент-серверное приложение
Реализовать что-то типо мессенджера, сообщение должно шифровать на стороне клиента
Клиент должен работать через терминал, шифрование должно быть симметричным
*/
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib") // Подключение библиотеки Winsock

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    WSADATA wsaData;
    SOCKET server_fd, client1_fd, client2_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    // Инициализация Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed." << std::endl;
        return 1;
    }

    // Создание сокета
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        std::cerr << "Socket creation failed." << std::endl;
        WSACleanup();
        return 1;
    }

    // Настройка адреса сервера
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Привязка сокета к адресу
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == SOCKET_ERROR) {
        std::cerr << "Bind failed." << std::endl;
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    // Ожидание подключения клиентов
    if (listen(server_fd, 2) == SOCKET_ERROR) {
        std::cerr << "Listen failed." << std::endl;
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    std::cout << "Waiting for clients to connect..." << std::endl;

    // Принятие подключения от первого клиента
    if ((client1_fd = accept(server_fd, (struct sockaddr *)&address, &addrlen)) == INVALID_SOCKET) {
        std::cerr << "Accept failed." << std::endl;
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }
    std::cout << "Client 1 connected!" << std::endl;

    // Принятие подключения от второго клиента
    if ((client2_fd = accept(server_fd, (struct sockaddr *)&address, &addrlen)) == INVALID_SOCKET) {
        std::cerr << "Accept failed." << std::endl;
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }
    std::cout << "Client 2 connected!" << std::endl;

    // Обмен сообщениями между клиентами
    while (true) {
        // Получение сообщения от клиента 1 и отправка клиенту 2
        memset(buffer, 0, BUFFER_SIZE);
        int valread = recv(client1_fd, buffer, BUFFER_SIZE, 0);
        if (valread <= 0) {
            std::cout << "Client 1 disconnected." << std::endl;
            break;
        }
        std::cout << "Client 1: " << buffer << std::endl;
        send(client2_fd, buffer, valread, 0);

        // Получение сообщения от клиента 2 и отправка клиенту 1
        memset(buffer, 0, BUFFER_SIZE);
        valread = recv(client2_fd, buffer, BUFFER_SIZE, 0);
        if (valread <= 0) {
            std::cout << "Client 2 disconnected." << std::endl;
            break;
        }
        std::cout << "Client 2: " << buffer << std::endl;
        send(client1_fd, buffer, valread, 0);
    }

    // Закрытие сокетов
    closesocket(client1_fd);
    closesocket(client2_fd);
    closesocket(server_fd);
    WSACleanup();
    return 0;
}