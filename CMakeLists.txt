cmake_minimum_required(VERSION 3.29)
project(cpp_clinet_server_chat)

set(CMAKE_CXX_STANDARD 14)

add_executable(cpp_clinet_server_chat
    client.cpp
    server.cpp)

target_link_libraries(cpp_clinet_server_chat ws2_32)