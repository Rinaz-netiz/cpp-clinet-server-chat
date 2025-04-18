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
#include <vector>
#define PORT 8080

Server::Server() {
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
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
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
    // мб надо очищать массивё
    int bytes = recv(fd, buff, sizeof(buff), 0);
    printf("Server received message from fd %d\n", fd);
    if (bytes == -1) {
        perror("recv");
        close(efd);
        close(listen_sock);
        exit(1);
    }
    if (bytes == 0) {
        if (epoll_ctl(efd, EPOLL_CTL_DEL, fd, nullptr)) {
            perror("epoll_ctr(DEL)");
            close(efd);
            close(listen_sock);
            return;
        }
        close(fd);
        for (int i = 0; i < clients.size(); ++i)
            if (fd == clients[i])
                clients.erase(clients.begin() + i);

        printf("Disconnected client (fd: %d)\n", fd);
    }

    buff[bytes] = '\0';
    // TODO обработку перессылки;
    msg_handler();
    send_msg(bytes, 0, fd);

}

void Server::send_msg(int size, int id, int fd) { // еще не сделал
    ssize_t sended=0;
    if (id == 0) {

        sended = send(fd, buff, size, MSG_NOSIGNAL);
        std::cout << "sended to client: " << fd << std::endl;
        if (sended == -1) {
            perror("send()");
            close(efd);
            close(listen_sock);
        }

        // for (int client:clients) {
        //     sended = send(client, buff, size, MSG_NOSIGNAL);
        //     std::cout << "sended to client: " << client << std::endl;
        //     if (sended == -1) {
        //         perror("send()");
        //         close(efd);
        //         close(listen_sock);
        //     }
        // }
        memset(buff, 0, sizeof(buff));
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


