#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>
#include <mutex>

class Logger {
public:
    enum class Status {
        INFO,
        WARNING,
        ERROR
    };

    Logger(const std::string& filename);
    ~Logger();

    void log(Status status, const std::string& message);

private:
    std::ofstream logfile;
    std::mutex mutex_;

    std::string currentDateTime();
    std::string statusToString(Status status);
};

#endif // LOGGER_H