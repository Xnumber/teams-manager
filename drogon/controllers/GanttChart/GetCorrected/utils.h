#pragma once
#include <drogon/drogon.h>

Json::Value createProjectItem(const drogon::orm::Row &project);
void addProjectsToGanttData(const drogon::orm::Result &projectResult, Json::Value &ganttData);

Json::Value createMilestoneItem(const drogon::orm::Row &milestone);
void addMilestonesToGanttData(const drogon::orm::Result &milestoneResult, Json::Value &ganttData);

Json::Value createBasicTaskGanttItem(const drogon::orm::Row &taskRawData, const std::string &startDate, const std::string &endDate);

void processAheadTask(    
    Json::Value &ganttData,
    const std::string &todayDate,
    Json::Value &aheadTasks
);

// Json::Value &newGanttData, 
void processDelayedTask(
    Json::Value &ganttData,
    const std::string &todayDate,
    Json::Value &delayedTasks
);

void addDelayedTaskMarker(const Json::Value &taskItem, Json::Value &ganttData);

void processTaskStatus(const drogon::orm::Row &taskRawData, const std::string &endDate, const std::string &todayDate, float estimatedWorkdays, Json::Value &taskItem, Json::Value &ganttData, Json::Value &aheadTasks, Json::Value &delayedTasks);

void processTask(const drogon::orm::Row &taskRawData, const std::string &taskStart, const std::string &todayDate, Json::Value &ganttData);

Json::Value *findGanttItemById(Json::Value &ganttData, const std::string &id);
void updateSuccessorDates(
    Json::Value *successorItem, 
    const std::string &newStartDate, 
    const drogon::orm::Result &taskResult,
    const float diffDays
);
Json::Value createDependencyItem(const drogon::orm::Row &dependency, const std::string &predecessorId, const std::string &successorId);


void correctDependencyDate(const drogon::orm::Row &dependency, Json::Value &ganttData, const drogon::orm::Result &taskResult, Json::Value &dependencyData, bool &existTaskStartEarlierThanDependencyEnd);



bool checkIfTaskStartEarlierThanDependencyEnd(const drogon::orm::Result &dependenciesResult, Json::Value &ganttData);
void addDependencyToGanttData(const drogon::orm::Row &dependency, Json::Value &ganttData, const drogon::orm::Result &taskResult, Json::Value &dependencyData, bool &existTaskStartEarlierThanDependencyEnd);
void addDependenciesToGanttData(const drogon::orm::Result &dependenciesResult, Json::Value &ganttData, const drogon::orm::Result &taskResult, Json::Value &dependencyData);
void correctDatesByDependencies(const drogon::orm::Result &dependenciesResult, Json::Value &ganttData, const drogon::orm::Result &taskResult, Json::Value &dependencyData);

std::string findLatestTaskEndDate(
    Json::Value &ganttData
);