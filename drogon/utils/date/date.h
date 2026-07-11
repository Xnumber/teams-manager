#pragma once

#include <string>

namespace date_utils {

std::string getTodayDate();

std::string getNextDate(const std::string& date);

std::string addWorkdays(const std::string& date, double estimatedWorkdays, double executorTimeRatio);
std::string minusWorkdays(const std::string& date, double estimatedWorkdays, double executorTimeRatio);

int daysBetweenWorkDays(const std::string& date1, const std::string& date2);

}
