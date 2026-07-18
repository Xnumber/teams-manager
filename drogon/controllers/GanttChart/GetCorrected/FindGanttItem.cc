#include "utils.h"

Json::Value *findGanttItemById(Json::Value &ganttData, const std::string &id)
{
    for (auto &item : ganttData)
    {
        if (item["id"].asString() == id)
        {
            return &item;
        }
    }
    return nullptr;
}

int *findGanttItemIndexById(Json::Value &ganttData, const std::string &id)
{
    for (int i = 0; i < ganttData.size(); ++i)
    {
        if (ganttData[i]["id"].asString() == id)
        {
            return new int(i);
        }
    }
    return nullptr;
}