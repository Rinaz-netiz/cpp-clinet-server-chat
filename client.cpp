#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib") // Подключение библиотеки Winsock

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    WSADATA wsaData;
    SOCKET sock = 0;
    struct sockaddr_in serv_addr;

    // Инициализация Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed." << std::endl;
        return 1;
    }

    // Создание сокета
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        std::cerr << "Socket creation failed." << std::endl;
        WSACleanup();
        return 1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Преобразование IP-адреса из текстового в бинарный формат
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported." << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // Подключение к серверу
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
        std::cerr << "Connection failed." << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    std::cout << "Connected to the server!" << std::endl;

    // Обмен сообщениями
    while (true) {
        // Отправка сообщения
        std::string message;
        std::cout << "You: ";
        std::getline(std::cin, message);
        send(sock, message.c_str(), message.length(), 0);

        // Получение сообщения
        char buffer[BUFFER_SIZE] = {0};
        int valread = recv(sock, buffer, BUFFER_SIZE, 0);
        if (valread <= 0) {
            std::cout << "Server disconnected." << std::endl;
            break;
        }
        std::cout << "Other client: " << buffer << std::endl;
    }

    // Закрытие сокета
    closesocket(sock);
    WSACleanup();
    return 0;
}