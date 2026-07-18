#include "utils.h"
#include "../../../utils/date/date.h"

void processAheadTask(
    Json::Value &ganttData,
    const std::string &todayDate,
    Json::Value &aheadTasks)
{
    // Json::Value currentAheadTask;
    float accumulatedAheadDays = 0;
    bool isAnyAheadTask = false;
    for (int i = 0; i < ganttData.size(); ++i)
    {
        float aheadDays = 0;
        float estimatedWorkdays = ganttData[i]["estimatedWorkdays"].asFloat();
        float executorTimeRatio = ganttData[i]["executorTimeRatio"].asFloat();
        std::string taskStart = ganttData[i]["start"].as<std::string>();
        std::string taskEnd = ganttData[i]["end"].as<std::string>();
        bool isCompleted = ganttData[i]["completed"].asBool();
        //  = ganttData[i]["parentId"].as<std::string>();

        if (
            isCompleted &&
            (taskEnd > todayDate) &&
            ganttData[i]["workItemType"].as<std::string>() == "TASK")
        {
            isAnyAheadTask = true;
            aheadTasks.append(ganttData[i]);
            ganttData[i]["ahead"] = true;

            float aheadDays = date_utils::daysBetweenWorkDays(todayDate, taskEnd);
            accumulatedAheadDays += aheadDays;
            if (aheadDays > estimatedWorkdays)
            {
                aheadDays = estimatedWorkdays;
                ganttData[i]["end"] = ganttData[i]["start"];
                ganttData[i]["estimatedWorkdays"] = 1;
                LOG_DEBUG << "ganttData[i][start]; " << ganttData[i]["start"].asString();
                // LOG_DEBUG << "ganttData[i][end]; " << ganttData[i]["end"].asString();
                // LOG_DEBUG << "todayDate " << todayDate;
                // LOG_DEBUG << "taskEnd " << taskEnd;
                // LOG_DEBUG << "aheadDays " << aheadDays;
                // LOG_DEBUG << "estimatedWorkdays " << estimatedWorkdays;
                LOG_DEBUG << "ganttData[i][start]; " << ganttData[i]["start"].asString();
                // LOG_DEBUG << "ganttData[i][end]; " << ganttData[i]["end"].asString();
            }
            else
            {
                // ganttData[i]["end"] = date_utils::minusWorkdays(taskEnd, aheadDays, 1);
                ganttData[i]["estimatedWorkdays"] = estimatedWorkdays - aheadDays;
            }

            ganttData[i]["aheadDays"] = aheadDays;
            // ganttData[i]["title"] = ganttData[i]["title"].as<std::string>() + " (超前" + std::to_string(static_cast<int>(aheadDays)) + "天)";
            ganttData[i]["title"] = ganttData[i]["title"].as<std::string>();
            ganttData[i]["isFirstAHeadTaskEndInParent"] = true;
            // currentAheadTask = ganttData[i];
            // }
            aheadTasks.append(ganttData[i]);
        }

        // newGanttData.append(ganttData[i]);
    }

    if (isAnyAheadTask)
    {
        for (int i = 1; i < ganttData.size(); ++i)
        {
            std::string previousEnd = ganttData[i - 1]["end"].asString();

            if (previousEnd.empty())
            {
                previousEnd = date_utils::getTodayDate();
                previousEnd = date_utils::minusWorkdays(previousEnd, 1, 1);
            }
            std::string currentStart = ganttData[i]["start"].asString();
            // std::string currentEnd = ganttData[i]["end"].asString();
            float estimatedWorkdays = ganttData[i]["estimatedWorkdays"].asFloat();
            float executorTimeRatio = ganttData[i]["executorTimeRatio"].asFloat();
            // float gap = date_utils::daysBetweenWorkDays(previousEnd, currentStart);
            // ganttData[i - 1]["parentId"] == ganttData[i]["parentId"] &&
            if (ganttData[i]["workItemType"] == "TASK")
            {
                LOG_DEBUG << "title" << ganttData[i - 1]["title"].asString();
                LOG_DEBUG << "title" << ganttData[i]["title"].asString();
                LOG_DEBUG << "previousEnd" << previousEnd;
                LOG_DEBUG << "currentStart" << currentStart;
                // LOG_DEBUG << "====== gap" << gap;

                if (!ganttData[i]["scheduled_start_date"].asString().empty())
                {
                    ganttData[i]["start"] = ganttData[i]["scheduled_start_date"].asString();
                }
                else
                {
                    ganttData[i]["start"] = date_utils::addWorkdays(previousEnd, 1, 1);
                }

                ganttData[i]["end"] = date_utils::addWorkdays(
                    currentStart,
                    estimatedWorkdays,
                    executorTimeRatio);
            }
        }
    }
}

void processDelayedTask(
    Json::Value &ganttData,
    const std::string &todayDate,
    Json::Value &delayedTasks,
    const drogon::orm::Result &dependenciesResult)
{
    // Json::Value currentDelayTask;
    bool isSpecifiedStartDateTaskFound = false;
    bool delayStarted = false;

    for (int i = 1; i < ganttData.size(); ++i)
    {
        float estimatedWorkdays = ganttData[i]["estimatedWorkdays"].asFloat();
        float executorTimeRatio = ganttData[i]["executorTimeRatio"].asFloat();
        std::string taskStart = ganttData[i]["start"].as<std::string>();
        std::string taskEnd = ganttData[i]["end"].as<std::string>();
        bool isCompleted = ganttData[i]["completed"].asBool();

        if (
            ganttData[i].isMember("scheduled_start_date"))
        {
            delayStarted = false;
        }

        if (
            !isCompleted &&
            (taskEnd < todayDate) &&
            ganttData[i]["workItemType"].as<std::string>() == "TASK" &&
            !delayStarted)
        {
            delayStarted = true;
            int delayDays = date_utils::daysBetweenWorkDays(taskEnd, todayDate);
            ganttData[i]["leadingDelayedTask"] = true;
            ganttData[i]["delayed"] = true;
            ganttData[i]["end"] = todayDate;
            ganttData[i]["delayDays"] = delayDays;
            ganttData[i]["estimatedWorkdays"] = estimatedWorkdays + delayDays;
            
            LOG_DEBUG << "TITLE  DELAYSTARTED = true; title " << ganttData[i]["title"].asString();
            LOG_DEBUG << "TITLE  DELAYSTARTED = true; scheduled_start_date " << ganttData[i]["scheduled_start_date"].empty();
            LOG_DEBUG << "TITLE  DELAYSTARTED = true; previous title " << ganttData[i-1]["title"].asString();
            // LOG_DEBUG << "TITLE  DELAYSTARTED = true; next title " << ganttData[i+1]["title"].asString();
            // LOG_DEBUG << "TITLE  DELAYSTARTED = true; start " << taskStart;
            // LOG_DEBUG << "TITLE  DELAYSTARTED = true; end " << taskEnd;
            // LOG_DEBUG << "TITLE  DELAYSTARTED = true; todayDate " << todayDate;
            // LOG_DEBUG << "TITLE  DELAYSTARTED = true; estimatedWorkdays " << estimatedWorkdays;
            // LOG_DEBUG << "TITLE  DELAYSTARTED = true; delayDays " << delayDays;
            // delayedTasks.append(ganttData[i]);
        }
    }

    for (int i = 1; i < ganttData.size(); ++i)
    {
        std::string previousEnd = ganttData[i - 1]["end"].asString();
        if (previousEnd.empty())
        {
            previousEnd = date_utils::getTodayDate();
            previousEnd = date_utils::minusWorkdays(previousEnd, 1, 1);
        }
        std::string currentStart = ganttData[i]["start"].asString();
        float estimatedWorkdays = ganttData[i]["estimatedWorkdays"].asFloat();
        float executorTimeRatio = ganttData[i]["executorTimeRatio"].asFloat();

        if (ganttData[i]["workItemType"] == "TASK")
        {
            // LOG_DEBUG << "title" << ganttData[i - 1]["title"].asString();
            // LOG_DEBUG << "title" << ganttData[i]["title"].asString();
            // LOG_DEBUG << "previousEnd" << previousEnd;
            // LOG_DEBUG << "currentStart" << currentStart;
            // LOG_DEBUG << "====== gap" << gap;
            std::string successorId = ganttData[i]["id"].as<std::string>();
            std::string dependencyPredecessorId;

            for (const drogon::orm::Row &dependency : dependenciesResult)
            {
                if (dependency["successor_id"].as<std::string>() == successorId)
                {
                    dependencyPredecessorId = dependency["predecessor_id"].as<std::string>();
                    break;
                }
            }

            if (!dependencyPredecessorId.empty())
            {
                // continue;
                Json::Value *predecessorItem = findGanttItemById(ganttData, dependencyPredecessorId);

                if (predecessorItem && predecessorItem->isMember("end") && !(*predecessorItem)["end"].asString().empty())
                {
                    ganttData[i]["start"] = date_utils::getNextDate((*predecessorItem)["end"].asString());
                }
                else if (!ganttData[i]["scheduled_start_date"].asString().empty())
                {
                    ganttData[i]["start"] = ganttData[i]["scheduled_start_date"].asString();
                }
                else
                {
                    ganttData[i]["start"] = date_utils::getNextDate(previousEnd);
                }

                ganttData[i]["end"] = date_utils::addWorkdays(
                    ganttData[i]["start"].asString(),
                    estimatedWorkdays,
                    executorTimeRatio);

                if(
                    (ganttData[i]["end"].asString() > todayDate) &&
                    ganttData[i]["workItemType"].as<std::string>() == "TASK"
                ) {
                    ganttData[i].removeMember("delayed");
                    ganttData[i].removeMember("delayDays");
                } else if(ganttData[i].isMember("leadingDelayedTask") && ganttData[i]["leadingDelayedTask"].asBool()) {
                    ganttData[i]["delayed"] = true;
                    ganttData[i]["delayDays"] = date_utils::daysBetweenWorkDays(ganttData[i]["end"].asString(), todayDate);
                    ganttData[i]["estimatedWorkdays"] = estimatedWorkdays + ganttData[i]["delayDays"].asFloat();   
                }
            }
            else if (!ganttData[i]["scheduled_start_date"].asString().empty())
            {
                ganttData[i]["start"] = ganttData[i]["scheduled_start_date"].asString();
                ganttData[i]["end"] = date_utils::addWorkdays(
                    ganttData[i]["start"].asString(),
                    estimatedWorkdays,
                    executorTimeRatio);
            }
            else
            {
                ganttData[i]["start"] = date_utils::getNextDate(previousEnd);
                ganttData[i]["end"] = date_utils::addWorkdays(
                    ganttData[i]["start"].asString(),
                    estimatedWorkdays,
                    executorTimeRatio);
            }

            if (
                !ganttData[i]["completed"].asBool() &&
                ganttData[i]["workItemType"].as<std::string>() == "TASK" &&
                ganttData[i]["end"].asString() < todayDate
            )
            {
                delayedTasks.append(ganttData[i]);
            }
        }
    }
}

void addDelayedTaskMarker(const Json::Value &taskItem, Json::Value &ganttData)
{
    Json::Value taskItemDelayed;
    taskItemDelayed["id"] = "delay-" + taskItem["id"].as<std::string>();
    taskItemDelayed["parentId"] = taskItem["parentId"].as<std::string>();
    // taskItemDelayed["title"] = taskItem["title"].as<std::string>() + " (落後)";
    taskItemDelayed["title"] = taskItem["title"].as<std::string>();
    taskItemDelayed["workItemType"] = "TASK";
    taskItemDelayed["priority"] = taskItem["priority"].as<int>();
    taskItemDelayed["completed"] = taskItem["completed"].as<bool>();
    taskItemDelayed["progress"] = taskItem["progress"].as<float>();
    taskItemDelayed["start"] = taskItem["end"];
    taskItemDelayed["end"] = date_utils::getTodayDate();
    // taskItemDelayed["delayed"] = true;
    ganttData.append(taskItemDelayed);
}

void processTaskStatus(const drogon::orm::Row &taskRawData, const std::string &endDate, const std::string &todayDate, float estimatedWorkdays, Json::Value &taskItem, Json::Value &ganttData, Json::Value &aheadTasks, Json::Value &delayedTasks)
{
    bool isCompleted = taskRawData["completed"].as<bool>();

    if (isCompleted && endDate > todayDate)
    {
        // processAheadTask(taskRawData, endDate, todayDate, estimatedWorkdays, aheadTasks);
        taskItem["ahead"] = true;
    }

    if (!isCompleted && endDate < todayDate)
    {
        // processDelayedTask(taskRawData, endDate, todayDate, delayedTasks);
        // taskItem["delayed"] = true;
        // addDelayedTaskMarker(taskItem, ganttData);
    }
}