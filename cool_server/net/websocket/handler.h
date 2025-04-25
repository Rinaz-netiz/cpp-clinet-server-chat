#pragma once
#include "frame.h"
#include <functional>
#include <memory>

namespace websocket {

class Connection {
public:
    using MessageCallback = std::function<void(const std::string&)>;
    using CloseCallback = std::function<void()>;

    virtual ~Connection() = default;
    
    virtual void sendText(const std::string& message) = 0;
    virtual void close() = 0;
    
    void setMessageCallback(MessageCallback cb) { on_message_ = cb; }
    void setCloseCallback(CloseCallback cb) { on_close_ = cb; }

protected:
    MessageCallback on_message_;
    CloseCallback on_close_;
};

class Handler {
public:
    virtual ~Handler() = default;
    
    virtual std::unique_ptr<Connection> handleHandshake(
        const std::string& request,
        std::string& response) = 0;
    
    virtual void handleData(
        Connection* connection,
        const std::vector<uint8_t>& data) = 0;
};

class RFC6455Handler : public Handler {
public:
    std::unique_ptr<Connection> handleHandshake(
        const std::string& request,
        std::string& response) override;
    
    void handleData(
        Connection* connection,
        const std::vector<uint8_t>& data) override;

private:
    bool parseHandshake(const std::string& request, std::string& key);
    std::string generateResponse(const std::string& key);
};

} // namespace websocket
