@echo off

rem Preparing for logging
if not exist "log" (
    mkdir log
    echo "Directory 'log' is created"
)

rem Building a project
if not exist "build" (
    mkdir build
    echo "Directory 'build' is created"
)

cd build
cmake -G "MinGW Makefiles" ..
mingw32-make

rem Run project
cd ..
start build/server.exe  # Запуск сервера в отдельном окне
ping 127.0.0.1 -n 3 >nul  # Задержка 2 секунды
start build/client.exe  # Запуск первого клиента
ping 127.0.0.1 -n 2 >nul  # Задержка 1 секунда перед запуском второго клиента
start build/client.exe  # Запуск второго клиента
