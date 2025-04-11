//
// Created by alister_mint on 04.04.25.
//

#ifndef SERVER_H
#define SERVER_H
#include <sys/epoll.h>

class Server {
    int listen_sock;
    int efd ;
    epoll_event ev{};

    void create_sock();
    void setup_epoll();
    void listen_con();
    void receive_msg();
    void send_msg();
    void msg_handler();

public:
    Server();
    ~Server();

    void run();
};


#endif //SERVER_H