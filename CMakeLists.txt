cmake_minimum_required(VERSION 3.29)
project(cpp_clinet_server_chat)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Логер
add_library(logger STATIC logger/logger.cpp)

# Утилиты (utils)
add_library(utils STATIC utils/utils.cpp)

# Клиентская часть
add_executable(client client.cpp des.cpp)
target_link_libraries(client ws2_32 logger utils)

# Серверная часть
add_executable(server server.cpp client_socket.cpp)
target_link_libraries(server ws2_32 logger)



add_custom_target(run_all
        COMMAND ${CMAKE_COMMAND} -E env cmd /c run_server_and_client.bat
        COMMENT "Запуск сервера и клиента через batch-скрипт"
        DEPENDS server client
)