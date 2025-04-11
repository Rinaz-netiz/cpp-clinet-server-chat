//
// Created by alister_mint on 04.04.25.
//

#include "../include/server.h"
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

Server::Server() {
    create_sock();
    setup_epoll();

    listen(listen_sock, 1024);
}

Server::~Server() {
    close(efd);
    close(listen_sock);
    //мб чето еще сделать надо
}

void Server::create_sock() {
    listen_sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr{};
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(8080);

    if (bind(listen_sock, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) == EADDRINUSE) {
        perror("bind");
        close(listen_sock);
        exit(1);
    }


}

void Server::setup_epoll() {
    efd = epoll_create(1024);

    ev.events = EPOLLIN | EPOLLPRI | EPOLLET;
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
    int count = epoll_wait(efd, &ev, 1, 1000);
    if (count == -1) {
        perror("epoll_wait");
        close(efd);
        close(listen_sock);
        exit(1);
    }
    if (count == 0)
        return;

    fd=ev.data.fd;
    if (fd==listen_sock)
        accept_client(fd);
    else
        receive_msg(fd);

    // TODO закрыть fd;



}

void Server::accept_client(int fd) {
    fd = accept(listen_sock, nullptr, nullptr);
    std::cout << "Server accepted connection " << fd << std::endl;
    ev.data.fd = fd;
    if (epoll_ctl(efd, EPOLL_CTL_ADD, fd, &ev)) {
        perror("epoll_ctl");
        close(efd);
        close(listen_sock);
        exit(1);
    }
}

void Server::receive_msg(int fd) {
    int bytes = recv(fd, buff, sizeof(buff), 0);
    if (bytes == -1) {
        perror("recv");
        close(efd);
        close(listen_sock);
        exit(1);
    }
    if (bytes == 0) {
        if (epoll_ctl(efd, EPOLL_CTL_DEL, fd, NULL)) {
            perror("epoll_ctr(DEL)");
            close(efd);
            close(listen_sock);
            return;
        }
        close(fd);
        printf("Disconnected client (fd: %d)\n", fd);
    }

    // TODO обработку перессылки;
    msg_handler();

}

void Server::send_msg(int size, int id) { // еще не сделал
    int sended = send(efd ,buff, size, 0);
    if (sended == -1) {
        perror("send()");
        close(efd);
        close(listen_sock);
    }
}

void Server::msg_handler() {

}


void Server::run() {
    std::cout << "Server listening" << std::endl;
    while (true) {
        listen_con();
    }
}


