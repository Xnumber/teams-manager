#include "BussinesCalendarCtrl.h"
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <jsoncpp/json/json.h>
#include <sstream>
#include <stdexcept>
#include <drogon/HttpResponse.h>
#include <drogon/orm/DbClient.h>

using namespace drogon;

namespace
{
std::string trimString(const std::string &value)
{
    const auto start = value.find_first_not_of(" \t\r\n");
    if (start == std::string::npos)
    {
        return "";
    }
    const auto end = value.find_last_not_of(" \t\r\n");
    return value.substr(start, end - start + 1);
}

bool parseDateParts(const std::string &input, int &year, int &month, int &day)
{
    std::string normalized = trimString(input);
    std::replace(normalized.begin(), normalized.end(), '/', '-');

    std::istringstream iss(normalized);
    char firstSeparator = '\0';
    char secondSeparator = '\0';
    if (!(iss >> year >> firstSeparator >> month >> secondSeparator >> day))
    {
        return false;
    }

    return firstSeparator == '-' && secondSeparator == '-' && year > 0 && month >= 1 && month <= 12 && day >= 1 && day <= 31;
}

std::string formatDateOnly(int year, int month, int day)
{
    std::ostringstream oss;
    oss << std::setw(4) << std::setfill('0') << year << '-'
        << std::setw(2) << std::setfill('0') << month << '-'
        << std::setw(2) << std::setfill('0') << day;
    return oss.str();
}

std::string normalizeTimeString(const std::string &input)
{
    const std::string value = trimString(input);
    if (value.empty())
    {
        return "";
    }

    int hour = 0;
    int minute = 0;
    int second = 0;
    char separator1 = '\0';
    char separator2 = ':';
    std::istringstream iss(value);
    if (value.find(':') != std::string::npos)
    {
        if (!(iss >> hour >> separator1 >> minute))
        {
            return value;
        }
        if (iss >> separator2 >> second)
        {
            // parsed hh:mm:ss
        }
        else
        {
            second = 0;
        }
        if (separator1 != ':')
        {
            return value;
        }
    }
    else
    {
        return value;
    }

    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << hour << ':'
        << std::setw(2) << std::setfill('0') << minute << ':'
        << std::setw(2) << std::setfill('0') << second;
    return oss.str();
}

std::string makeUtcTimestampString(const std::string &dateString, const std::string &timeString, bool allDayEvent)
{
    if (allDayEvent || trimString(timeString).empty())
    {
        return dateString + " 00:00:00+08";
    }

    return dateString + " " + normalizeTimeString(timeString) + "+00";
}

bool parseFlexibleBool(const Json::Value &value, bool defaultValue = true)
{
    if (value.isBool())
    {
        return value.asBool();
    }
    if (value.isString())
    {
        std::string text = trimString(value.asString());
        std::transform(text.begin(), text.end(), text.begin(), [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
        if (text == "true" || text == "1" || text == "yes")
        {
            return true;
        }
        if (text == "false" || text == "0" || text == "no")
        {
            return false;
        }
    }
    return defaultValue;
}

bool isBlankTemplateRow(const Json::Value &item)
{
    if (!item.isObject())
    {
        return false;
    }

    const std::string subject = trimString(item.get("Subject", "").asString());
    const std::string startDate = trimString(item.get("Start Date", "").asString());
    const std::string startTime = trimString(item.get("Start Time", "").asString());
    const std::string endDate = trimString(item.get("End Date", "").asString());
    const std::string endTime = trimString(item.get("End Time", "").asString());
    const std::string allDayEvent = trimString(item.get("All Day Event", "").asString());
    const std::string description = trimString(item.get("Description", "").asString());
    const std::string location = trimString(item.get("Location", "").asString());

    return subject.empty() &&
           startDate.empty() &&
           startTime.empty() &&
           endDate.empty() &&
           endTime.empty() &&
           allDayEvent.empty() &&
           description.empty() &&
           location.empty();
}
} // namespace


void BussinesCalendarCtrl::importBussinesCalendar(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
{
    Json::Value response;
    auto dbClient = drogon::app().getDbClient("teams_manager");

    Json::Value payload;
    {
        Json::CharReaderBuilder builder;
        std::string errors;
        const std::string body(req->getBody().data(), req->getBody().size());
        std::istringstream input(body);
        if (!Json::parseFromStream(builder, input, &payload, &errors) || !payload.isArray())
        {
            response["result"] = "error";
            response["message"] = "Invalid JSON array body";
            auto resp = HttpResponse::newHttpJsonResponse(response);
            resp->setStatusCode(k400BadRequest);
            callback(resp);
            return;
        }
    }

    Json::Value filteredPayload(Json::arrayValue);
    int skippedCount = 0;
    for (Json::ArrayIndex index = 0; index < payload.size(); ++index)
    {
        const Json::Value &item = payload[index];
        if (!item.isObject())
        {
            response["result"] = "error";
            response["message"] = "Invalid record at index " + std::to_string(index);
            auto resp = HttpResponse::newHttpJsonResponse(response);
            resp->setStatusCode(k400BadRequest);
            callback(resp);
            return;
        }

        if (isBlankTemplateRow(item))
        {
            ++skippedCount;
            continue;
        }

        filteredPayload.append(item);
    }

    if (filteredPayload.empty())
    {
        response["result"] = "ok";
        response["data"]["processed_count"] = 0;
        response["data"]["skipped_count"] = skippedCount;
        response["data"]["rows"] = Json::Value(Json::arrayValue);
        auto resp = HttpResponse::newHttpJsonResponse(response);
        callback(resp);
        return;
    }

    Json::Value importedRows(Json::arrayValue);
    int processedCount = 0;

    int targetYear = 0;
    for (Json::ArrayIndex index = 0; index < filteredPayload.size(); ++index)
    {
        const Json::Value &item = filteredPayload[index];

        int year = 0;
        int startMonth = 0;
        int startDay = 0;
        int endYear = 0;
        int endMonth = 0;
        int endDay = 0;
        const std::string startDateText = item.get("Start Date", "").asString();
        const std::string endDateText = item.get("End Date", "").asString();

        if (!parseDateParts(startDateText, year, startMonth, startDay))
        {
            response["result"] = "error";
            response["message"] = "Invalid Start Date at index " + std::to_string(index);
            auto resp = HttpResponse::newHttpJsonResponse(response);
            resp->setStatusCode(k400BadRequest);
            callback(resp);
            return;
        }

        if (!parseDateParts(endDateText, endYear, endMonth, endDay))
        {
            response["result"] = "error";
            response["message"] = "Invalid End Date at index " + std::to_string(index);
            auto resp = HttpResponse::newHttpJsonResponse(response);
            resp->setStatusCode(k400BadRequest);
            callback(resp);
            return;
        }

        if (year != endYear)
        {
            response["result"] = "error";
            response["message"] = "Start Date and End Date must be in the same year at index " + std::to_string(index);
            auto resp = HttpResponse::newHttpJsonResponse(response);
            resp->setStatusCode(k400BadRequest);
            callback(resp);
            return;
        }

        if (targetYear == 0)
        {
            targetYear = year;
        }
        else if (targetYear != year)
        {
            response["result"] = "error";
            response["message"] = "All imported records must be in the same year";
            auto resp = HttpResponse::newHttpJsonResponse(response);
            resp->setStatusCode(k400BadRequest);
            callback(resp);
            return;
        }
    }

    auto trans = dbClient->newTransaction();

    try
    {
        for (Json::ArrayIndex index = 0; index < filteredPayload.size(); ++index)
        {
            const Json::Value &item = filteredPayload[index];

            const std::string subject = item.get("Subject", "").asString();
            const std::string startDateText = item.get("Start Date", "").asString();
            const std::string startTimeText = item.get("Start Time", "").asString();
            const std::string endDateText = item.get("End Date", "").asString();
            const std::string endTimeText = item.get("End Time", "").asString();
            const std::string description = item.get("Description", "").asString();
            const std::string location = item.get("Location", "").asString();
            const bool allDayEvent = parseFlexibleBool(item.get("All Day Event", Json::Value(true)));

            int year = 0;
            int startMonth = 0;
            int startDay = 0;
            int endYear = 0;
            int endMonth = 0;
            int endDay = 0;
            if (!parseDateParts(startDateText, year, startMonth, startDay))
            {
                throw std::runtime_error("Invalid Start Date at index " + std::to_string(index));
            }
            if (!parseDateParts(endDateText, endYear, endMonth, endDay))
            {
                throw std::runtime_error("Invalid End Date at index " + std::to_string(index));
            }

            if (year != targetYear || endYear != targetYear)
            {
                throw std::runtime_error("All imported records must be in the same year");
            }

            const std::string normalizedStartDate = formatDateOnly(year, startMonth, startDay);
            const std::string normalizedEndDate = formatDateOnly(endYear, endMonth, endDay);
            const std::string startTimestamp = makeUtcTimestampString(normalizedStartDate, startTimeText, allDayEvent);
            std::cout << "startTimestamp: " << startTimestamp << std::endl;
            const std::string endTimestamp = makeUtcTimestampString(normalizedEndDate, endTimeText, allDayEvent);
            std::cout << "endTimestamp: " << endTimestamp << std::endl;

            trans->execSqlSync(
                R"(
                    WITH updated AS (
                        UPDATE business_calendar
                        SET year = ($1::integer)::smallint,
                            subject = $2,
                            start_time = NULLIF($4, '')::time,
                            end_date = $5::timestamptz,
                            end_time = NULLIF($6, '')::time,
                            all_day_event = $7,
                            description = NULLIF($8, ''),
                            location = NULLIF($9, '')
                        WHERE start_date::date = $3::date
                        RETURNING id
                    )
                    INSERT INTO business_calendar (
                        year,
                        subject,
                        start_date,
                        start_time,
                        end_date,
                        end_time,
                        all_day_event,
                        description,
                        location
                    )
                    SELECT
                        ($1::integer)::smallint,
                        $2,
                        $3::timestamptz,
                        NULLIF($4, '')::time,
                        $5::timestamptz,
                        NULLIF($6, '')::time,
                        $7,
                        NULLIF($8, ''),
                        NULLIF($9, '')
                    WHERE NOT EXISTS (SELECT 1 FROM updated)
                )",
                year,
                subject,
                startTimestamp,
                normalizeTimeString(startTimeText),
                startTimestamp,
                normalizeTimeString(endTimeText),
                allDayEvent,
                description,
                location);

            Json::Value importedItem;
            importedItem["subject"] = subject;
            importedItem["start_date"] = normalizedStartDate;
            importedItem["end_date"] = normalizedEndDate;
            importedItem["all_day_event"] = allDayEvent;
            importedRows.append(importedItem);
            ++processedCount;
        }

        response["result"] = "ok";
        response["data"]["processed_count"] = processedCount;
        response["data"]["skipped_count"] = skippedCount;
        response["data"]["rows"] = importedRows;
        auto resp = HttpResponse::newHttpJsonResponse(response);
        callback(resp);
    }
    catch (const std::exception &e)
    {
        response["result"] = "error";
        response["message"] = e.what();
        auto resp = HttpResponse::newHttpJsonResponse(response);
        resp->setStatusCode(k500InternalServerError);
        callback(resp);
    }
}
