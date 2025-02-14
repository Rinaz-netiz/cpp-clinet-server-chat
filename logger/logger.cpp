#include "logger.h"

#include <map>
#include <chrono>
#include <iomanip>
#include <sstream>

// for easily print
std::map<LogLevel, std::string> logLevelMap = {
    {LogLevel::INFO, "INFO"},
    {LogLevel::WARNING, "WARNING"},
    {LogLevel::ERROR_, "ERROR"},
    {LogLevel::DEBUG, "DEBUG"}
};

Logger::Logger() {
    logFile.open("log/log.txt", std::ios::app);  // Открываем файл для дозаписи
    if (!logFile) {
        std::cerr << "Ошибка открытия файла логов!" << std::endl;
    }
}

Logger::~Logger() {
    if (logFile.is_open()) {
        logFile.close();  // Закрываем файл при завершении программы
    }
}

Logger& Logger::getInstance() {
    static Logger instance;  // Гарантированно создается только один раз
    return instance;
}

void Logger::setLogLevel(LogLevel level) {
    currentLevel = level;
}

// В методе log добавим получение текущего времени
void Logger::log(LogLevel level, const std::string& message) {
    if (level < currentLevel) return;  // Фильтруем ненужные логи

    // Получаем текущее время
    auto now = std::chrono::system_clock::now();
    std::time_t timeNow = std::chrono::system_clock::to_time_t(now);
    std::tm* timeInfo = std::localtime(&timeNow);

    // Форматируем время в строку
    std::stringstream timeStream;
    timeStream << std::put_time(timeInfo, "%Y-%m-%d %H:%M:%S");

    std::string timestamp = timeStream.str();
    std::string logMessage = "[" + timestamp + "] [" + levelToString(level) + "] " + message;

    // Блокируем доступ в многопоточной среде
    std::lock_guard<std::mutex> lock(logMutex);

    std::cout << logMessage << std::endl;  // Выводим в консоль
    if (logFile.is_open()) {
        logFile << logMessage << std::endl;  // Записываем в файл
    }
}


std::string Logger::levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR_: return "ERROR";
        case LogLevel::DEBUG: return "DEBUG";
        default: return "UNKNOWN";
    }
}


/*
#include "Logger.h"

int main() {
    Logger& logger = Logger::getInstance();  // Получаем логгер

    logger.setLogLevel(LogLevel::DEBUG);  // Устанавливаем уровень логирования

    logger.log(LogLevel::INFO, "Программа запущена");
    logger.log(LogLevel::DEBUG, "Тестовое сообщение для отладки");
    logger.log(LogLevel::WARNING, "Внимание! Возможная проблема");
    logger.log(LogLevel::ERROR_, "Ошибка! Что-то пошло не так");

    return 0;
}

 */