#include "Logger.h"

#include <iostream>

Logger& Logger::getLogger() {
    static Logger instance;
    return instance;
}

void Logger::info(const std::string& message) {
    log("INFO", message);
}

void Logger::error(const std::string& message) {
    log("ERROR", message);
}

void Logger::debug(const std::string& message) {
    if (DEBUG) {
        log("DEBUG", message);
    }
}

void Logger::log(const std::string& typeOfError, const std::string& message) {
    // For demonstration purposes, we'll just print to the console
    std::cout << "[" << typeOfError << "] " << message << std::endl;
}
