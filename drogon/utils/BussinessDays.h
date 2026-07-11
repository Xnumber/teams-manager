#pragma once
#include <ctime>
#include <string>
#include <vector>

// Add business days to a start time_t, skipping Saturdays, Sundays, and holidays.
std::time_t addBusinessDays(std::time_t start, long days);

// Fetch holiday dates for a given year from business_calendar.
std::vector<std::string> getHolidays(int year);

// Check whether the given day exists in business_calendar as a holiday.
bool isHoliday(std::time_t day);

// Check whether the given day exists in a pre-fetched holiday list.
bool isHoliday(std::time_t day, const std::vector<std::string> &holidays);
