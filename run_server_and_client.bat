@echo off
start build/server.exe  # Запуск сервера в отдельном окне
ping 127.0.0.1 -n 3 >nul  # Задержка 2 секунды
start build/client.exe  # Запуск первого клиента
ping 127.0.0.1 -n 2 >nul  # Задержка 1 секунда перед запуском второго клиента
start build/client.exe  # Запуск второго клиента
