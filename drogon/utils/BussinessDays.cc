#include "BussinessDays.h"
#include "utils/TimeToDateString.h"
#include <drogon/drogon.h>
#include <drogon/orm/DbClient.h>
#include <algorithm>
#include <ctime>
#include <vector>

using namespace drogon;
using namespace drogon::orm;

namespace
{
int getYearFromTime(std::time_t day)
{
    std::tm tmBuf;
    localtime_r(&day, &tmBuf);
    return tmBuf.tm_year + 1900;
}
} // namespace

std::vector<std::string> getHolidays(int year) {
    DbClientPtr dbClient = drogon::app().getDbClient("teams_manager");

    try {
        Result result = dbClient->execSqlSync(
            R"(
                SELECT to_char(start_date::date, 'YYYY-MM-DD') AS holiday_date
                FROM business_calendar
                WHERE year = $1
                  AND subject IS NOT NULL
                  AND btrim(subject) <> ''
                ORDER BY start_date ASC
            )",
            year);

        std::vector<std::string> holidays;
        holidays.reserve(result.size());
        for (const auto &row : result) {
            holidays.emplace_back(row["holiday_date"].as<std::string>());
        }
        return holidays;
    } catch (const std::exception &) {
        return {};
    }
}

bool isHoliday(std::time_t day, const std::vector<std::string> &holidays) {
    const std::string dateString = timeToDateString(day);
    // std::cout << "Checking if " << timeToDateString(day) << " is a holiday..." << std::endl;
    return std::find(holidays.begin(), holidays.end(), dateString) != holidays.end();
}

bool isHoliday(std::time_t day) {
    const int year = getYearFromTime(day);
    const std::vector<std::string> holidays = getHolidays(year);
    // std::cout << "Checking if " << timeToDateString(day) << " is a holiday..." << std::endl;
    return isHoliday(day, holidays);
}

//
std::time_t addBusinessDays(std::time_t start, long days) {
    if (days <= 0) return start;
    std::time_t cur = start;
    long added = 0;
    int cachedYear = 0;
    std::vector<std::string> holidays;
    std::tm res;
    int year = res.tm_year + 1900;
    holidays = getHolidays(year);

    for (size_t i = 0; i < holidays.size(); i++)
    {
        std::cout << "Holiday: " << holidays[i] << std::endl;
        /* code */
    }
    


    while (added < days) {
        cur += 24 * 60 * 60; // advance one day
        localtime_r(&cur, &res);
        int wday = res.tm_wday; // 0 = Sunday, 6 = Saturday
        if (year != cachedYear) {
            cachedYear = year;
        }
        // std::cout << "day: " << timeToDateString(cur) << std::endl;

        if (wday == 0 || wday == 6 || isHoliday(cur, holidays)) {
            // std::cout << "Skipping non-business day: " << timeToDateString(cur) << std::endl;
            continue; // skip weekends and holidays
        }
        ++added;
    }
    return cur;
}