#pragma once
#include <ctime>
#include <string>

namespace WeekTimeUtils {

// 獲取本週星期一的日期（若為六日則取下週一）
inline std::string getThisMonday() {
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);
    int wday = tm.tm_wday;
    if (wday == 0) wday = 7; // 星期日為0，轉成7
    
    if (wday >= 6) { // 星期六或星期日
        tm.tm_mday += (8 - wday); // 下週一
    } else {
        tm.tm_mday -= (wday - 1); // 本週一
    }
    
    std::time_t monday = std::mktime(&tm);
    char buf[11];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d", std::localtime(&monday));
    return std::string(buf);
}

// 獲取本週星期五的日期（若為六日則取下週五）
inline std::string getThisFriday() {
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);
    int wday = tm.tm_wday;
    if (wday == 0) wday = 7; // 星期日為0，轉成7
    
    if (wday >= 6) { // 星期六或星期日
        tm.tm_mday += (8 - wday); // 先到下週一
        tm.tm_mday += 4; // 下週五
    } else {
        tm.tm_mday -= (wday - 1); // 先到本週一
        tm.tm_mday += 4; // 本週五
    }
    
    std::time_t friday = std::mktime(&tm);
    char buf[11];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d", std::localtime(&friday));
    return std::string(buf);
}





// 獲取下週星期一的日期（若為六日則取下下週一）
inline std::string getNextMonday() {
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);
    int wday = tm.tm_wday;
    if (wday == 0) wday = 7; // 星期日為0，轉成7
    
    if (wday >= 6) { // 星期六或星期日
        tm.tm_mday += (15 - wday); // 下下週一
    } else {
        tm.tm_mday += (8 - wday); // 下週一
    }
    
    std::time_t monday = std::mktime(&tm);
    char buf[11];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d", std::localtime(&monday));
    return std::string(buf);
}

// 獲取下週星期五的日期（若為六日則取下下週五）
inline std::string getNextFriday() {
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);
    int wday = tm.tm_wday;
    if (wday == 0) wday = 7; // 星期日為0，轉成7
    
    if (wday >= 6) { // 星期六或星期日
        tm.tm_mday += (15 - wday); // 先到下下週一
        tm.tm_mday += 4; // 下下週五
    } else {
        tm.tm_mday += (8 - wday); // 先到下週一
        tm.tm_mday += 4; // 下週五
    }
    
    std::time_t friday = std::mktime(&tm);
    char buf[11];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d", std::localtime(&friday));
    return std::string(buf);
}

// 獲取本月第一天的日期
inline std::string getThisMonthStart() {
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);
    tm.tm_mday = 1;
    std::mktime(&tm);
    char buf[11];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d", &tm);
    return std::string(buf);
}

// 獲取本月最後一天的日期
inline std::string getThisMonthEnd() {
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);
    tm.tm_mon += 1;  // 下個月
    tm.tm_mday = 0;  // 下個月第0天 = 本月最後一天
    std::mktime(&tm);
    char buf[11];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d", &tm);
    return std::string(buf);
}

} // namespace WeekTimeUtils
