#include "handler.h"
#include "frame.h"
#include <sstream>
#include <random>
#include <base64.h> // Нужна реализация base64
#include <sha1.h>   // Нужна реализация SHA1

namespace websocket {

namespace {
    const std::string MAGIC_GUID = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
}

std::unique_ptr<Connection> RFC6455Handler::handleHandshake(
    const std::string& request, 
    std::string& response) {
    
    std::string client_key;
    if (!parseHandshake(request, client_key)) {
        return nullptr;
    }
    
    response = generateResponse(client_key);
    return std::make_unique<ConnectionImpl>(); // ConnectionImpl нужно реализовать
}

bool RFC6455Handler::parseHandshake(const std::string& request, std::string& key) {
    std::istringstream iss(request);
    std::string line;
    
    // Проверяем первую строку
    std::getline(iss, line);
    if (line.find("GET") == std::string::npos) return false;
    
    // Ищем Sec-WebSocket-Key
    while (std::getline(iss, line)) {
        if (line.find("Sec-WebSocket-Key:") != std::string::npos) {
            size_t start = line.find(':') + 1;
            while (start < line.size() && (line[start] == ' ' || line[start] == '\t')) start++;
            size_t end = line.find('\r', start);
            key = line.substr(start, end - start);
            return true;
        }
    }
    
    return false;
}

std::string RFC6455Handler::generateResponse(const std::string& key) {
    std::string combined = key + MAGIC_GUID;
    std::string sha1 = SHA1(combined); // Нужна реализация SHA1
    std::string accept = base64_encode(sha1); // Нужна реализация base64
    
    std::ostringstream oss;
    oss << "HTTP/1.1 101 Switching Protocols\r\n"
        << "Upgrade: websocket\r\n"
        << "Connection: Upgrade\r\n"
        << "Sec-WebSocket-Accept: " << accept << "\r\n"
        << "\r\n";
    
    return oss.str();
}

void RFC6455Handler::handleData(Connection* connection, const std::vector<uint8_t>& data) {
    FrameHeader header;
    if (!Frame::parseHeader(data, header)) {
        connection->close();
        return;
    }
    
    std::vector<uint8_t> payload(data.begin() + Frame::headerSize(header), data.end());
    std::string message = Frame::decodePayload(header, payload);
    
    switch (header.opcode) {
        case Opcode::Text:
            if (connection->on_message_) {
                connection->on_message_(message);
            }
            break;
        case Opcode::Close:
            connection->close();
            break;
        case Opcode::Ping:
            // TODO: Send pong
            break;
        default:
            break;
    }
}

} // namespace websocket
