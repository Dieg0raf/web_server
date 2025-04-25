#ifndef LOGGER_H
#define LOGGER_H

#include <string>

constexpr bool DEBUG = false;

class Logger {
   public:
    static Logger& getLogger();
    void info(const std::string& message);
    void error(const std::string& message);
    void debug(const std::string& message);

   private:
    void log(const std::string& typeOfError, const std::string& message);
};

#endif
