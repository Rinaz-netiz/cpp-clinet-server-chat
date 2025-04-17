#include "Database.h"
#include <iostream>

Database::Database() : connection(nullptr) {
    if (!connect()) {
        exit(EXIT_FAILURE);
    }
}

Database::~Database() {
    if (connection) {
        mysql_close(connection);
    }
}

void Database::logError(const std::string& message) {
    std::cerr << "Database Error: " << message 
              << " (MySQL error: " << mysql_error(connection) << ")" << std::endl;
}

bool Database::connect() {
    connection = mysql_init(nullptr);
    if (!connection) {
        logError("Failed to initialize MySQL connection");
        return false;
    }

    if (!mysql_real_connect(connection, host.c_str(), user.c_str(),
                          password.c_str(), db.c_str(), 0, nullptr, 0)) {
        logError("Failed to connect to database");
        return false;
    }

    return true;
}

bool Database::addUser(const std::string& login, const std::string& passwordHash) {
    std::string query = "INSERT INTO users (login, password_hash) VALUES (?, ?)";
    
    MYSQL_STMT* stmt = mysql_stmt_init(connection);
    if (!stmt) {
        logError("Failed to initialize statement");
        return false;
    }

    if (mysql_stmt_prepare(stmt, query.c_str(), query.length())) {
        logError("Failed to prepare statement");
        mysql_stmt_close(stmt);
        return false;
    }

    MYSQL_BIND bind[2];
    memset(bind, 0, sizeof(bind));

    bind[0].buffer_type = MYSQL_TYPE_STRING;
    bind[0].buffer = (void*)login.c_str();
    bind[0].buffer_length = login.length();

    bind[1].buffer_type = MYSQL_TYPE_STRING;
    bind[1].buffer = (void*)passwordHash.c_str();
    bind[1].buffer_length = passwordHash.length();

    if (mysql_stmt_bind_param(stmt, bind)) {
        logError("Failed to bind parameters");
        mysql_stmt_close(stmt);
        return false;
    }

    if (mysql_stmt_execute(stmt)) {
        logError("Failed to execute query");
        mysql_stmt_close(stmt);
        return false;
    }

    mysql_stmt_close(stmt);
    return true;
}

bool Database::authenticateUser(int fd, const std::string& login, const std::string& passwordHash) {
    std::string query = "SELECT id FROM users WHERE login = ? AND password_hash = ?";
    
    MYSQL_STMT* stmt = mysql_stmt_init(connection);
    if (!stmt) {
        logError("Failed to initialize statement");
        return false;
    }

    if (mysql_stmt_prepare(stmt, query.c_str(), query.length())) {
        logError("Failed to prepare statement");
        mysql_stmt_close(stmt);
        return false;
    }

    MYSQL_BIND bind[2];
    memset(bind, 0, sizeof(bind));

    bind[0].buffer_type = MYSQL_TYPE_STRING;
    bind[0].buffer = (void*)login.c_str();
    bind[0].buffer_length = login.length();

    bind[1].buffer_type = MYSQL_TYPE_STRING;
    bind[1].buffer = (void*)passwordHash.c_str();
    bind[1].buffer_length = passwordHash.length();

    if (mysql_stmt_bind_param(stmt, bind)) {
        logError("Failed to bind parameters");
        mysql_stmt_close(stmt);
        return false;
    }

    if (mysql_stmt_execute(stmt)) {
        logError("Failed to execute query");
        mysql_stmt_close(stmt);
        return false;
    }

    int user_id = 0;
    MYSQL_BIND result;
    memset(&result, 0, sizeof(result));
    result.buffer_type = MYSQL_TYPE_LONG;
    result.buffer = &user_id;

    mysql_stmt_bind_result(stmt, &result);
    mysql_stmt_store_result(stmt);

    if (mysql_stmt_fetch(stmt)) {
        mysql_stmt_close(stmt);
        return false;
    }

    fd_to_user[fd] = user_id;
    mysql_stmt_close(stmt);
    return true;
}

bool Database::saveMessage(int fd, const std::string& message) {
    auto it = fd_to_user.find(fd);
    if (it == fd_to_user.end()) {
        return false;
    }

    std::string query = "INSERT INTO messages (user_id, message) VALUES (?, ?)";
    
    MYSQL_STMT* stmt = mysql_stmt_init(connection);
    if (!stmt) {
        logError("Failed to initialize statement");
        return false;
    }

    if (mysql_stmt_prepare(stmt, query.c_str(), query.length())) {
        logError("Failed to prepare statement");
        mysql_stmt_close(stmt);
        return false;
    }

    int user_id = it->second;
    MYSQL_BIND bind[2];
    memset(bind, 0, sizeof(bind));

    bind[0].buffer_type = MYSQL_TYPE_LONG;
    bind[0].buffer = &user_id;

    bind[1].buffer_type = MYSQL_TYPE_STRING;
    bind[1].buffer = (void*)message.c_str();
    bind[1].buffer_length = message.length();

    if (mysql_stmt_bind_param(stmt, bind)) {
        logError("Failed to bind parameters");
        mysql_stmt_close(stmt);
        return false;
    }

    if (mysql_stmt_execute(stmt)) {
        logError("Failed to execute query");
        mysql_stmt_close(stmt);
        return false;
    }

    mysql_stmt_close(stmt);
    return true;
}

void Database::disconnectUser(int fd) {
    fd_to_user.erase(fd);
}