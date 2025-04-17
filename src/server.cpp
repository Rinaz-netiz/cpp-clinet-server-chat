//
// Created by alister_mint on 04.04.25.
//

#include "../include/server.h"
#include <netinet/in.h>
#include <cstdio>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <strings.h>
#include <cstring>
#include <arpa/inet.h>
#include <cerrno>
#include <cstdlib>
#include <sys/epoll.h>
#include <csignal>
#include <iostream>
#include <fcntl.h>
#define PORT 9999

Server::Server() : db() {
    create_sock();
    setup_epoll();
}

Server::~Server() {
    close(efd);
    close(listen_sock);
    //мб чето еще сделать надо
}

void Server::create_sock() {
    listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    if (bind(listen_sock, reinterpret_cast<sockaddr *>(&addr), sizeof(addr))) {
        perror("bind");
        close(listen_sock);
        exit(1);
    }


}

void Server::setup_epoll() {
    efd = epoll_create1(0);

    ev.events = EPOLLIN; // | EPOLLET; // | EPOLLPRI    ;
    ev.data.fd = listen_sock;

    if (epoll_ctl(efd, EPOLL_CTL_ADD, listen_sock, &ev) < 0) {
        perror("epoll_ctl");
        close(efd);
        close(listen_sock);
        exit(1);
    }
}


void Server::listen_con() {
    int fd;
    int count = epoll_wait(efd, events, 1024, 1000);

    if (count == -1) {
        perror("epoll_wait");
        close(efd);
        close(listen_sock);
        exit(1);
    }


    for (int i = 0; i < count; i++) {
        fd=events[i].data.fd;
        if (fd == listen_sock) {
            accept_client(fd);
        }
        else
            receive_msg(fd);
    }
}

void Server::accept_client(int &fd) {
    fd = accept(listen_sock, nullptr, nullptr);
    std::cout << "Server accepted connection " << fd << std::endl;

    fcntl(fd, F_SETFL, O_NONBLOCK);

    ev.events = EPOLLIN; // | EPOLLET | EPOLLOUT;
    ev.data.fd = fd;

    if (epoll_ctl(efd, EPOLL_CTL_ADD, fd, &ev)) {
        perror("epoll_ctl");
        close(efd);
        close(listen_sock);
        exit(1);
    }
    clients.emplace_back(fd);
}

void Server::receive_msg(int &fd) {
    int bytes = recv(fd, buff, sizeof(buff), 0);
    if (bytes == -1) {
        perror("recv");
        return;
    }
    if (bytes == 0) {
        db.disconnectUser(fd);
        if (epoll_ctl(efd, EPOLL_CTL_DEL, fd, nullptr)) {
            perror("epoll_ctr(DEL)");
        }
        close(fd);
        printf("Disconnected client (fd: %d)\n", fd);
        return;
    }

    std::string received(buff, bytes);
    
    // Обработка команд (регистрация/аутентификация)
    if (received.find("/register") == 0 || received.find("/login") == 0) {
        handle_command(fd, received);
        return;
    }

    // Сохранение обычного сообщения
    if (!db.saveMessage(fd, received)) {
        std::string error = "Error saving message";
        send(fd, error.c_str(), error.size(), 0);
        return;
    }

    send_msg(bytes, 0);
}

void Server::send_msg(int size, int id) { // еще не сделал
    int sended=0;
    if (id == 0) {
        for (int client:clients) {
            sended = send(client, buff, size, MSG_NOSIGNAL);
            printf("sended = %d\n", sended);
            if (sended == -1) {
                perror("send()");
                close(efd);
                close(listen_sock);
            }
        }
    }


}

void Server::msg_handler() {

}


void Server::run() {
    std::cout << "Server listening" << std::endl;
    listen(listen_sock, 1024);
    fcntl(listen_sock, F_SETFL, O_NONBLOCK);

    while (true) {
        listen_con();
    }
}

void Server::handle_command(int fd, const std::string& command) {
    size_t space_pos = command.find(' ');
    if (space_pos == std::string::npos) {
        std::string response = "Invalid command format";
        send(fd, response.c_str(), response.size(), 0);
        return;
    }

    std::string cmd = command.substr(0, space_pos);
    std::string args = command.substr(space_pos + 1);
    
    size_t colon_pos = args.find(':');
    if (colon_pos == std::string::npos) {
        std::string response = "Invalid arguments format (login:password required)";
        send(fd, response.c_str(), response.size(), 0);
        return;
    }

    std::string login = args.substr(0, colon_pos);
    std::string password = args.substr(colon_pos + 1);

    if (cmd == "/register") {
        if (db.addUser(login, password)) {
            std::string response = "Registration successful";
            send(fd, response.c_str(), response.size(), 0);
        } else {
            std::string response = "Registration failed";
            send(fd, response.c_str(), response.size(), 0);
        }
    }
    else if (cmd == "/login") {
        if (db.authenticateUser(fd, login, password)) {
            std::string response = "Login successful";
            send(fd, response.c_str(), response.size(), 0);
        } else {
            std::string response = "Login failed";
            send(fd, response.c_str(), response.size(), 0);
        }
    }
}

Server::~Server() {
    for (int fd : clients) {
        db.disconnectUser(fd);
        close(fd);
    }
    close(efd);
    close(listen_sock);
}
