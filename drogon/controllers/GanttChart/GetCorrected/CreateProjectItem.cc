#include "utils.h"

Json::Value createProjectItem(const drogon::orm::Row &project)
{
    Json::Value item;
    item["id"] = project["id"].as<std::string>();
    item["parentId"] = 0;
    item["title"] = project["name"].as<std::string>();
    item["workItemType"] = "PROJECT";
    item["priority"] = project["priority"].as<int>();
    return item;
}

void addProjectsToGanttData(const drogon::orm::Result &projectResult, Json::Value &ganttData)
{
    for (const drogon::orm::Row &project : projectResult)
    {
        ganttData.append(createProjectItem(project));
    }
}
