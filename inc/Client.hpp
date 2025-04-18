#ifndef CHATCLIENT_H
#define CHATCLIENT_H

#include <string>
#include <thread>

class ChatClient {
public:
    ChatClient(const std::string& name, const std::string& ip, int port);
    ~ChatClient();

    void run();

private:
    void sendLoop();
    void receiveLoop();
    void connectToServer();
    void closeConnection();

    std::string name;
    std::string ip;
    int port;
    int sock;
    std::thread sender;
    std::thread receiver;
    bool running;
};

#endif
