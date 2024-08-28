#include "Logger.h"
#include <iomanip>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iostream>

Logger::Logger(const std::string& filename) : logfile(filename, std::ios::out | std::ios::app) {
    if (!logfile.is_open()) {
        std::cerr << "Error: Unable to open log file." << std::endl;
    }
}

Logger::~Logger() {
    if (logfile.is_open()) {
        logfile.close();
    }
}

void Logger::log(Status status, const std::string& message) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (logfile.is_open()) {
        logfile << currentDateTime() << " - " << statusToString(status) << " - " << message << std::endl;
    }
}

std::string Logger::currentDateTime() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&in_time_t);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string Logger::statusToString(Status status) {
    switch (status) {
        case Status::INFO: return "INFO";
        case Status::WARNING: return "WARNING";
        case Status::ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}