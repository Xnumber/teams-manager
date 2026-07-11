#include "utils.h"

Json::Value createMilestoneItem(const drogon::orm::Row &milestone)
{
    Json::Value item;
    item["id"] = milestone["id"].as<std::string>();
    item["parentId"] = milestone["project_id"].as<std::string>();
    item["title"] = milestone["name"].as<std::string>();
    item["workItemType"] = "MILESTONE";
    item["priority"] = milestone["priority"].as<int>();
    return item;
}

void addMilestonesToGanttData(const drogon::orm::Result &milestoneResult, Json::Value &ganttData)
{
    for (const drogon::orm::Row &milestone : milestoneResult)
    {
        ganttData.append(createMilestoneItem(milestone));
    }
}
