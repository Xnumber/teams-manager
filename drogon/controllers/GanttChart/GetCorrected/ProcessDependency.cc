#include "utils.h"
#include "../../../utils/date/date.h"

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

void updateSuccessorDates(
    Json::Value *successorItem, 
    const std::string &newStartDate,
    const drogon::orm::Result &taskResult,
    const float diffDays
) {
    (*successorItem)["start"] = newStartDate;

    float estimatedWorkdays = 0;
    float executorTimeRatio = 0;
    std::string successorId = (*successorItem)["id"].asString();

    for (const drogon::orm::Row &taskRawData : taskResult)
    {
        if (taskRawData["id"].as<std::string>() == successorId)
        {
            estimatedWorkdays = taskRawData["estimated_workdays"].as<float>();
            executorTimeRatio = taskRawData["excecutor_time_ratio"].as<float>();
            break;
        }
    }

    if (estimatedWorkdays > 0)
    {
        std::string newEndDate = date_utils::addWorkdays(newStartDate, estimatedWorkdays, executorTimeRatio);
        (*successorItem)["end"] = newEndDate;
    }
}

Json::Value createDependencyItem(const drogon::orm::Row &dependency, const std::string &predecessorId, const std::string &successorId)
{
    Json::Value item;
    item["id"] = dependency["id"].as<std::string>();
    item["predecessorId"] = predecessorId;
    item["predecessorType"] = dependency["predecessor_type"].as<std::string>();
    item["successorId"] = successorId;
    item["successorType"] = dependency["successor_type"].as<std::string>();
    return item;
}

void addDependencyToGanttData(
    const drogon::orm::Row &dependency, 
    Json::Value &ganttData, 
    const drogon::orm::Result &taskResult, 
    Json::Value &dependencyData, 
    bool &existTaskStartEarlierThanDependencyEnd
) {
    std::string predecessorId = dependency["predecessor_id"].as<std::string>();
    std::string successorId = dependency["successor_id"].as<std::string>();

    dependencyData.append(createDependencyItem(dependency, predecessorId, successorId));
}


void addDependenciesToGanttData(
    const drogon::orm::Result &dependenciesResult,
    Json::Value &ganttData,
    const drogon::orm::Result &taskResult,
    Json::Value &dependencyData
) {
    bool existTaskStartEarlierThanDependencyEnd = false;
    for (const drogon::orm::Row &dependency : dependenciesResult)
    {
        addDependencyToGanttData(
            dependency, 
            ganttData, 
            taskResult, 
            dependencyData, 
            existTaskStartEarlierThanDependencyEnd
        );
    }
}





bool correctDatesByDependency(
    const drogon::orm::Row &dependency, 
    Json::Value &ganttData, 
    const drogon::orm::Result &taskResult, 
    Json::Value &dependencyData
    // bool &existTaskStartEarlierThanDependencyEnd
) {
    bool isChanged = false;
    std::string predecessorId = dependency["predecessor_id"].as<std::string>();
    std::string successorId = dependency["successor_id"].as<std::string>();
    
    Json::Value *predecessorItem = findGanttItemById(ganttData, predecessorId);
    Json::Value *successorItem = findGanttItemById(ganttData, successorId);
    std::string parentId = successorItem ? (*successorItem)["parentId"].asString() : "";
    
    int *successorIndex = findGanttItemIndexById(ganttData, successorId);
    
    if (predecessorItem && successorItem && predecessorItem->isMember("end") &&
    (*predecessorItem)["end"].asString() > (*successorItem)["start"].asString())
    {
        isChanged = true;
        float diffDays = date_utils::daysBetweenWorkDays(
            (*successorItem)["start"].asString(),
            (*predecessorItem)["end"].asString()
        );

        // existTaskStartEarlierThanDependencyEnd = true;
        // LOG_DEBUG << "P TITLE " << (*predecessorItem)["title"].asString();
        // LOG_DEBUG << "P start " << (*predecessorItem)["start"].asString();
        // LOG_DEBUG << "P end " << (*predecessorItem)["end"].asString();
        // LOG_DEBUG << "S task TITLE " << (*successorItem)["title"].asString();
        // LOG_DEBUG << "S task start " << (*successorItem)["start"].asString();
        // LOG_DEBUG << "S task end " << (*successorItem)["end"].asString();
        
        // std::string newStartDate = date_utils::getNextDate((*predecessorItem)["end"].asString());
        
        LOG_DEBUG << "diffDays " << diffDays;
        // LOG_DEBUG << "diffDays " << diffDays;
        // LOG_DEBUG << "diffDays " << diffDays;
        // LOG_DEBUG << "diffDays " << diffDays;
        // LOG_DEBUG << "diffDays " << diffDays;
        // LOG_DEBUG << "diffDays " << diffDays;
        // LOG_DEBUG << "diffDays " << diffDays;

        // updateSuccessorDates(
        //     successorItem, 
        //     newStartDate, 
        //     taskResult,
        //     diffDays
        // );

        // std::string predecessorEnd = (*predecessorItem)["end"].asString();
        // std::string successorStart = date_utils::getNextDate(predecessorEnd);


        
        
        int firstIndex = -1;
        int lastIndex = -1;
        for (int i = 0; i < ganttData.size(); ++i)
        {
            if (
                ganttData[i]["parentId"].asString() == parentId &&
                ganttData[i]["priority"].asInt() >= (*successorItem)["priority"].asInt()
            )
            {
                if (firstIndex == -1)
                {
                    firstIndex = i;
                }
                lastIndex = i;
            }
        }
        LOG_DEBUG << "firstIndex " << firstIndex;
        LOG_DEBUG << "lastIndex " << lastIndex;
        int startIndex = *successorIndex + 1;
        LOG_DEBUG << "startIndex " << startIndex;

        ganttData[*successorIndex]["start"] = date_utils::getNextDate((*predecessorItem)["end"].asString());
        ganttData[*successorIndex]["end"] = date_utils::addWorkdays(
            ganttData[*successorIndex]["end"].asString(),
            diffDays,
            1
        );


        for (int i = startIndex; i <= lastIndex; ++i)
        {
            float executorTimeRatio = ganttData[i]["executorTimeRatio"].asFloat();
            std::string previousEnd = ganttData[i - 1]["end"].asString();
            std::string newStartDate = date_utils::getNextDate(previousEnd);
            std::string endDate = ganttData[i]["end"].asString();
            std::string newEndDate = date_utils::addWorkdays(endDate, diffDays, executorTimeRatio);
            ganttData[i]["start"] = newStartDate;
            ganttData[i]["end"] = newEndDate;
        }
    }

    return isChanged;
}



void correctDatesByDependencies(
    const drogon::orm::Result &dependenciesResult,
    Json::Value &ganttData,
    const drogon::orm::Result &taskResult,
    Json::Value &dependencyData
) {

    int count = 0;

    // bool existTaskStartEarlierThanDependencyEnd = checkIfTaskStartEarlierThanDependencyEnd(dependenciesResult, ganttData);
    bool changed = true;
    // while (existTaskStartEarlierThanDependencyEnd)
    while (changed)
    {

        changed = false;
        count += 1;
        for (const drogon::orm::Row &dependency : dependenciesResult)
        {
            changed = correctDatesByDependency(
                dependency, 
                ganttData, 
                taskResult, 
                dependencyData
            );
        }
        
        
        // existTaskStartEarlierThanDependencyEnd = checkIfTaskStartEarlierThanDependencyEnd(dependenciesResult, ganttData);

        if (count > 2000) {
            throw std::runtime_error("Exceeded maximum iterations (2000) while correcting dates by dependencies. Possible circular dependency detected.");
        }
    }
}
bool checkIfTaskStartEarlierThanDependencyEnd(
    const drogon::orm::Result &dependenciesResult,
    Json::Value &ganttData
) {
    for (const drogon::orm::Row &dependency : dependenciesResult)
    {
        std::string predecessorId = dependency["predecessor_id"].as<std::string>();
        std::string successorId = dependency["successor_id"].as<std::string>();
        
        Json::Value *predecessorItem = findGanttItemById(ganttData, predecessorId);
        Json::Value *successorItem = findGanttItemById(ganttData, successorId);

        if (predecessorItem && successorItem && predecessorItem->isMember("end") &&
            (*predecessorItem)["end"].asString() > (*successorItem)["start"].asString())
        {
            return true;
        }
    }
    return false;
}