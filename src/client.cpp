// ChatClient.cpp
#include "../inc/Client.hpp"
#include "../inc/Logger.hpp"
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <cstring>
#include <iostream>

#define BUF_SIZE 1024

ChatClient::ChatClient(const std::string& name, const std::string& ip, int port)
    : name("[" + name + "]"), ip(ip), port(port), running(true) {
    connectToServer();
}

ChatClient::~ChatClient() {
    closeConnection();
}

void ChatClient::run() {
    sender = std::thread(&ChatClient::sendLoop, this);
    receiver = std::thread(&ChatClient::receiveLoop, this);

    sender.join();
    receiver.join();
}

void ChatClient::connectToServer() {
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) Logger::fatal("socket() failed");

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    serv_addr.sin_port = htons(port);

    if (connect(sock, (sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        Logger::fatal("connect() failed");

    std::string intro = "#new client: " + name;
    send(sock, intro.c_str(), intro.size() + 1, 0);
}

void ChatClient::closeConnection() {
    if (running) {
        running = false;
        close(sock);
    }
}

void ChatClient::sendLoop() {
    std::string msg;
    while (running) {
        std::getline(std::cin, msg);
        if (msg == "quit" || msg == "Quit") {
            closeConnection();
            exit(0);
        }
        std::string formatted = name + " " + msg;
        send(sock, formatted.c_str(), formatted.size() + 1, 0);
    }
}

void ChatClient::receiveLoop() {
    char buffer[BUF_SIZE + 128];  // extra space for name
    while (running) {
        int len = recv(sock, buffer, sizeof(buffer), 0);
        if (len <= 0) {
            Logger::error("Connection closed by server.\n");
            exit(0);
        }
        std::cout << buffer << std::endl;
    }
}
