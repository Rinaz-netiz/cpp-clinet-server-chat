#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <fstream>
#include <string>
#include <mutex>

enum class LogLevel { INFO, WARNING, ERROR_, DEBUG };

class Logger {
private:
    std::ofstream logFile;
    std::mutex logMutex;  // Для потокобезопасности
    LogLevel currentLevel = LogLevel::INFO;  // Уровень логирования

    Logger();  // Приватный конструктор (синглтон)
    ~Logger(); // Закрытие файла в деструкторе

public:
    // Удаляем копирование и присваивание
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    static Logger& getInstance();  // Получить единственный экземпляр

    void setLogLevel(LogLevel level);  // Установить уровень логирования
    void log(LogLevel level, const std::string& message);  // Запись в лог

    static std::string levelToString(LogLevel level);  // Преобразование уровня в строку
};

#endif // LOGGER_H
