@echo off
start cmake-build-debug/server.exe  # Запуск сервера в отдельном окне
timeout /t 2      # Ожидаем 1 секунду, чтобы сервер успел запуститься
start cmake-build-debug/client.exe  # Запуск клиента 1
timeout /t 1
start cmake-build-debug/client.exe  # Запуск клиента 2