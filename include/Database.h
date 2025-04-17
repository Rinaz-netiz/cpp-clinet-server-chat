#pragma once
#include <mysql/mysql.h>
#include <string>
#include <unordered_map>

class Database {
private:
    MYSQL* connection;
    std::unordered_map<int, int> fd_to_user;  // Соответствие fd ↔ user_id
    
    const std::string host = "localhost";
    const std::string user = "messenger_user";
    const std::string password = "secure_password123";
    const std::string db = "messenger_db";

    void logError(const std::string& message);

public:
    Database();
    ~Database();

    bool connect();
    bool addUser(const std::string& login, const std::string& passwordHash);
    bool authenticateUser(int fd, const std::string& login, const std::string& passwordHash);
    bool saveMessage(int fd, const std::string& message);
    void disconnectUser(int fd);
};