#include "utils.h"

Json::Value createBasicTaskGanttItem(const drogon::orm::Row &taskRawData, const std::string &startDate, const std::string &endDate)
{
    Json::Value item;
    item["start"] = startDate;
    item["end"] = endDate;
    item["id"] = taskRawData["id"].as<std::string>();
    item["parentId"] = taskRawData["milestone_id"].as<std::string>();
    item["title"] = taskRawData["name"].as<std::string>();
    item["workItemType"] = "TASK";
    item["priority"] = taskRawData["priority"].as<int>();
    item["completed"] = taskRawData["completed"].as<bool>();
    item["progress"] = taskRawData["progress"].as<float>();
    item["estimatedWorkdays"] = taskRawData["estimated_workdays"].as<float>();
    item["executorTimeRatio"] = taskRawData["excecutor_time_ratio"].as<float>();
    item["priority"] = taskRawData["priority"].as<int>();
    return item;
}