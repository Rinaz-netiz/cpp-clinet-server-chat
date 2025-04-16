//
// Created by alister_mint on 04.04.25.
//

#ifndef SERVER_H
#define SERVER_H
#include <sys/epoll.h>
#include <vector>

class Server {
    int listen_sock; // сам  сокет
    int efd ;        // пул epoll
    epoll_event ev{}, events[1024];
    char buff[1024];
    std::vector<int> clients;

    void create_sock();
    void setup_epoll();
    void listen_con();
    void accept_client(int &fd);
    void receive_msg(int &fd);
    void send_msg(int size, int id);
    void msg_handler();

public:
    Server();
    ~Server();

    void run();
};


#endif //SERVER_H
