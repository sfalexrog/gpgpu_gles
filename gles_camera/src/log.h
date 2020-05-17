#pragma once
#include <iostream>
#include <sstream>
#include <chrono>

namespace Logger
{

enum Severity
{
LOG_DEBUG = 0,
LOG_INFO = 1,
LOG_WARN = 2,
LOG_ERROR = 3,
LOG_FATAL = 4,
Severity_SIZE
};

inline std::string sevStr(const Severity &s)
{
    const std::string sev_strs[] = {
    " [DEBUG] ", " [ INFO] ", " [ WARN] ", " [  ERR] ", " [FATAL] ", " [UNKN!] "};
    size_t sev_idx = static_cast<size_t>(s);
    if (sev_idx < static_cast<size_t>(Severity::Severity_SIZE))
    {
        return sev_strs[sev_idx];
    }
    return sev_strs[static_cast<size_t>(Severity_SIZE)];
}

class Logger
{
private:
    Severity s;
    std::stringstream ss;
    std::chrono::time_point<std::chrono::system_clock> timestamp;
public:
    Logger(const Severity& severity) : s(severity), timestamp(std::chrono::system_clock::now()) {}
    ~Logger() {
        std::cout << timestamp.time_since_epoch().count() << sevStr(s) << ss.str() << std::endl;
    }
    std::stringstream& get() {return ss;}
    static Severity& minSeverity()
    {
        static Severity minSeverity = LOG_DEBUG;
        return minSeverity;
    }
};

}

#define LOG(severity) if (Logger::LOG_ ## severity < Logger::Logger::minSeverity()) {} else Logger::Logger(Logger::LOG_ ## severity).get()
#define LOG_FUNC(severity) LOG(severity) << __PRETTY_FUNCTION__ << "@" << __LINE__ << " "
