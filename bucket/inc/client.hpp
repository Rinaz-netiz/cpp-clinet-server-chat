#ifndef CLIENT_HPP

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

class Client {
private:
    int client_socket;
    sockaddr_in server_addr;

public:
  Client();

  void recieve_message();
  void send_message();
};

#define CLIENT_HPP

#endif //CLIENT_HPP
