cmake_minimum_required(VERSION 3.29)
project(cpp_clinet_server_chat)

set(CMAKE_CXX_STANDARD 14)

# Клиентская часть
add_executable(client client.cpp)
target_link_libraries(client ws2_32)  # Подключение библиотеки Winsock для клиента

# Серверная часть
add_executable(server server.cpp)
target_link_libraries(server ws2_32)  # Подключение библиотеки Winsock для сервера
