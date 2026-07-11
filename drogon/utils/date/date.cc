#include "date.h"
#include <chrono>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <cstdio>
#include <string>
namespace date_utils {
std::string getTodayDate() {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::gmtime(&time_t_now), "%Y-%m-%d");
    return ss.str();
}

std::string getNextDate(const std::string& date) {
    return date_utils::addWorkdays(date, 2.0, 1.0);
}

std::string addWorkdays(const std::string& date, double estimatedWorkdays, double executorTimeRatio) {
    // Parse input date string (format: 2026-06-27)
    std::tm tm = {};
    std::istringstream ss(date);
    ss >> std::get_time(&tm, "%Y-%m-%d");

    time_t time_t_date = std::mktime(&tm);
    std::chrono::_V2::system_clock::time_point day_point = std::chrono::system_clock::from_time_t(time_t_date);

    // Calculate actual workdays to add: estimatedWorkdays / executorTimeRatio
    double workdaysToAdd = estimatedWorkdays / executorTimeRatio;
    int wholeDays = static_cast<int>(std::ceil(workdaysToAdd));

    // Add workdays (skip weekends)
    int daysAdded = 0;
    int currentDay = 0;

    while (daysAdded < wholeDays) {
        currentDay++;
        std::chrono::_V2::system_clock::time_point next_day = day_point + std::chrono::hours(24 * currentDay);
        time_t next_time_t = std::chrono::system_clock::to_time_t(next_day);
        std::tm* next_tm = std::gmtime(&next_time_t);

        // Check if it's a weekday (Monday=1 to Friday=5, Saturday=6, Sunday=0)
        int dayOfWeek = next_tm->tm_wday;
        if (dayOfWeek != 0 && dayOfWeek != 6) {  // Skip Sunday (0) and Saturday (6)
            daysAdded++;
        }
    }

    auto result_day = day_point + std::chrono::hours(24 * currentDay);
    auto result_time_t = std::chrono::system_clock::to_time_t(result_day);

    std::stringstream result_ss;
    result_ss << std::put_time(std::gmtime(&result_time_t), "%Y-%m-%d");
    return result_ss.str();
}

std::string minusWorkdays(const std::string& date, double estimatedWorkdays, double executorTimeRatio) {
    // Parse input date string (format: 2026-06-27)
    std::tm tm = {};
    std::istringstream ss(date);
    ss >> std::get_time(&tm, "%Y-%m-%d");

    time_t time_t_date = std::mktime(&tm);
    std::chrono::_V2::system_clock::time_point day_point = std::chrono::system_clock::from_time_t(time_t_date);

    // Calculate actual workdays to minus: estimatedWorkdays / executorTimeRatio
    double workdaysToMinus = estimatedWorkdays / executorTimeRatio;
    int wholeDays = static_cast<int>(std::ceil(workdaysToMinus));

    // Minus workdays (skip weekends)
    int daysSubtracted = 0;
    int currentDay = 0;

    while (daysSubtracted < wholeDays) {
        currentDay++;
        std::chrono::_V2::system_clock::time_point next_day = day_point - std::chrono::hours(24 * currentDay);
        time_t next_time_t = std::chrono::system_clock::to_time_t(next_day);
        std::tm* next_tm = std::gmtime(&next_time_t);

        // Check if it's a weekday (Monday=1 to Friday=5, Saturday=6, Sunday=0)
        int dayOfWeek = next_tm->tm_wday;
        if (dayOfWeek != 0 && dayOfWeek != 6) {  // Skip Sunday (0) and Saturday (6)
            daysSubtracted++;
        }
    }

    auto result_day = day_point - std::chrono::hours(24 * currentDay);
    auto result_time_t = std::chrono::system_clock::to_time_t(result_day);

    std::stringstream result_ss;
    result_ss << std::put_time(std::gmtime(&result_time_t), "%Y-%m-%d");
    return result_ss.str();
}

int daysBetweenWorkDays(const std::string& date1, const std::string& date2) {
    std::tm tm1 = {}, tm2 = {};
    std::istringstream ss1(date1), ss2(date2);
    ss1 >> std::get_time(&tm1, "%Y-%m-%d");
    ss2 >> std::get_time(&tm2, "%Y-%m-%d");

    time_t t1 = std::mktime(&tm1);
    time_t t2 = std::mktime(&tm2);

    auto day1 = std::chrono::system_clock::from_time_t(t1);
    auto day2 = std::chrono::system_clock::from_time_t(t2);

    int workdays = 0;
    auto current = day1;

    // Iterate through each day and count only weekdays (Monday-Friday)
    while (current < day2) {
        current = current + std::chrono::hours(24);
        time_t current_time_t = std::chrono::system_clock::to_time_t(current);
        std::tm* current_tm = std::gmtime(&current_time_t);

        int dayOfWeek = current_tm->tm_wday;
        if (dayOfWeek != 0 && dayOfWeek != 6) {  // Skip Sunday (0) and Saturday (6)
            workdays++;
        }
    }

    return workdays;
}


}
