#pragma once
#include <ctime>
#include <string>
#include <iomanip>
#include <sstream>

inline std::string timeToDateString(std::time_t timestamp) {
    std::tm* tm = std::localtime(&timestamp);
    std::ostringstream oss;
    oss << std::put_time(tm, "%Y-%m-%d");
    return oss.str();
}
