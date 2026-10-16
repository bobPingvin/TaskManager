#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>

class Logger {
public:
    static void init(const std::string& logFilePath) {
        getInstance().logFile_.open(logFilePath, std::ios::app);
    }

    static void log(const std::string& level, const std::string& msg) {
        auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::tm tm_buf{};
        localtime_r(&now, &tm_buf);

        std::ostringstream ss;
        ss << "[" << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S") << "] [" << level << "] " << msg;
        std::string line = ss.str();

        std::cout << line << std::endl;
        if (getInstance().logFile_.is_open()) {
            getInstance().logFile_ << line << std::endl;
        }
    }

    static void info(const std::string& msg)  { log("INFO", msg); }
    static void warn(const std::string& msg)  { log("WARN", msg); }
    static void error(const std::string& msg) { log("ERROR", msg); }

private:
    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }
    std::ofstream logFile_;
};
