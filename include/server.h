#ifndef SERVER_H
#define SERVER_H

#include <sys/epoll.h>
#include <vector>
#include <string>
#include "Database.h"

class Server {
private:
    int listen_sock;
    int efd;
    epoll_event ev{}, events[1024];
    char buff[1024];
    std::vector<int> clients;
    Database db;

    void create_sock();
    void setup_epoll();
    void listen_con();
    void accept_client(int &fd);
    void receive_msg(int &fd);
    void send_msg(int size, int id);
    void handle_command(int fd, const std::string& command);

public:
    Server();
    ~Server();

    void run();
};

#endif
