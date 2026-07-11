#include "utils.h"
#include "../../../utils/date/date.h"

void processAheadTask(
    Json::Value &ganttData,
    const std::string &todayDate,
    Json::Value &aheadTasks)
{
    // Json::Value currentAheadTask;
    float accumulatedAheadDays = 0;

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
            // if (
            //     !currentAheadTask.isNull() &&
            //     currentAheadTask["parentId"].as<std::string>() == ganttData[i]["parentId"].as<std::string>()
            // ) {
            // ganttData[i]["delayed"] = true;
            aheadTasks.append(ganttData[i]);
            // newGanttData.append(ganttData[i]);
            // continue;
            // } else {
            ganttData[i]["ahead"] = true;

            float aheadDays = date_utils::daysBetweenWorkDays(todayDate, taskEnd);
            accumulatedAheadDays += aheadDays;
            if (aheadDays > estimatedWorkdays)
            {
                aheadDays = estimatedWorkdays;
                ganttData[i]["end"] = ganttData[i]["start"];
                // LOG_DEBUG << "todayDate " << todayDate;
                // LOG_DEBUG << "taskEnd " << taskEnd;
                // LOG_DEBUG << "aheadDays " << aheadDays;
                // LOG_DEBUG << "estimatedWorkdays " << estimatedWorkdays;
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
                ganttData[i]["end"] = date_utils::minusWorkdays(taskEnd, aheadDays, 1);
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

    // 如果前一個parentId跟後一個的一樣，但是前一個end跟後一個start，不小於等於1的話
    // 則將後一個start設定成前一個end的日期加一天
    for (int i = 2; i < ganttData.size(); ++i)
    {
        std::string previousEnd = ganttData[i - 1]["end"].asString();
        std::string currentStart = ganttData[i]["start"].asString();
        std::string currentEnd = ganttData[i]["end"].asString();
        float gap = date_utils::daysBetweenWorkDays(previousEnd, currentStart);
        if (
            ganttData[i - 1]["parentId"] == ganttData[i]["parentId"] &&
            ganttData[i - 1]["workItemType"] == "TASK" &&
            ganttData[i]["workItemType"] == "TASK" &&
            gap > 1)
        {
            LOG_DEBUG << "title" << ganttData[i - 1]["title"].asString();
            LOG_DEBUG << "title" << ganttData[i]["title"].asString();
            LOG_DEBUG << "previousEnd" << previousEnd;
            LOG_DEBUG << "currentStart" << currentStart;
            LOG_DEBUG << "====== gap" << gap;
            ganttData[i]["start"] = date_utils::addWorkdays(previousEnd, 1, 1);
            ganttData[i]["end"] = date_utils::minusWorkdays(currentEnd, gap - 2, 1);
        }
        // else {

        //     LOG_DEBUG << "====== ";
        //     LOG_DEBUG << "title" <<  ganttData[i - 1]["title"].asString();
        //     LOG_DEBUG << "title" <<  ganttData[i]["title"].asString();
        //     LOG_DEBUG << "====== previousEnd";
        //     LOG_DEBUG << "previousEnd" <<  previousEnd;
        //     LOG_DEBUG << "currentStart" <<  currentStart;
        // }
    }
}
// if (
//     ganttData[i]["isFirstAHeadTaskEndInParent"].asBool() &&
//     ganttData[i]["completed"].asBool())
// {
//     std::string firstDelayedTaskEnd = ganttData[i]["end"].as<std::string>();
//     std::string parentId = ganttData[i]["parentId"].as<std::string>();
//     float aheadDays = ganttData[i]["aheadDays"].asFloat();

//     for (int j = i + 1; j < ganttData.size(); ++j)
//     {
//         if (ganttData[j]["parentId"].as<std::string>() == parentId)
//         {
//             std::string originalStart = ganttData[j]["start"].as<std::string>();
//             std::string originalEnd = ganttData[j]["end"].as<std::string>();

//             std::string newStart = date_utils::minusWorkdays(originalStart, aheadDays, 1.0);
//             std::string newEnd = date_utils::minusWorkdays(originalEnd, aheadDays, 1.0);
//             // newGanttData[j]["title"] = newGanttData[j]["title"].as<std::string>() + " (落後" + std::to_string(static_cast<int>(delayDays)) + "天)";
//             ganttData[j]["title"] = ganttData[j]["title"].as<std::string>() + " (推前" + std::to_string(static_cast<int>(aheadDays)) + "天)";
//             ganttData[j]["start"] = newStart;
//             ganttData[j]["end"] = newEnd;
//             ganttData[j]["ahead"] = true;

//             // LOG_DEBUG << " = = = = == = == Adjusted task ";
//             // LOG_DEBUG << "=========Adjusted task ";
//             // LOG_DEBUG << "=========Adjusted task ";
//             // LOG_DEBUG << "=========Adjusted task ";
//         }
//     }
// }

void processDelayedTask(
    Json::Value &ganttData,
    // Json::Value &newGanttData,
    const std::string &todayDate,
    Json::Value &delayedTasks)
{
    Json::Value currentDelayTask;

    for (int i = 0; i < ganttData.size(); ++i)
    {
        float estimatedWorkdays = ganttData[i]["estimatedWorkdays"].asFloat();
        float executorTimeRatio = ganttData[i]["executorTimeRatio"].asFloat();
        std::string taskStart = ganttData[i]["start"].as<std::string>();
        std::string taskEnd = ganttData[i]["end"].as<std::string>();
        bool isCompleted = ganttData[i]["completed"].asBool();
        //  = ganttData[i]["parentId"].as<std::string>();

        if (
            !isCompleted &&
            (taskEnd < todayDate) &&
            ganttData[i]["workItemType"].as<std::string>() == "TASK")
        {
            if (
                !currentDelayTask.isNull() &&
                currentDelayTask["parentId"].as<std::string>() == ganttData[i]["parentId"].as<std::string>())
            {
                // ganttData[i]["delayed"] = true;
                delayedTasks.append(ganttData[i]);
                // newGanttData.append(ganttData[i]);
                continue;
            }
            else
            {
                ganttData[i]["delayed"] = true;
                ganttData[i]["end"] = todayDate;
                ganttData[i]["delayDays"] = date_utils::daysBetweenWorkDays(taskEnd, todayDate);
                // ganttData[i]["title"] = ganttData[i]["title"].as<std::string>() + " (落後" + std::to_string(static_cast<int>(ganttData[i]["delayDays"].asFloat())) + "天)";
                ganttData[i]["title"] = ganttData[i]["title"].as<std::string>();

                ganttData[i]["isFirstDelayedTaskEndInParent"] = true;
                currentDelayTask = ganttData[i];
            }
            delayedTasks.append(ganttData[i]);
        }

        // newGanttData.append(ganttData[i]);
    }

    // 找到每一個parentId下的，isFirstDelayedTaskEndInParent為true, completed為false的任務作為 firstDelayedTask，
    // 並將其後的同一個parentId下的任務，利用firstDelayedTask的delayDays，重新計算其後的start date與end date
    for (int i = 0; i < ganttData.size(); ++i)
    {
        if (
            ganttData[i]["isFirstDelayedTaskEndInParent"].asBool() &&
            !ganttData[i]["completed"].asBool())
        {
            std::string firstDelayedTaskEnd = ganttData[i]["end"].as<std::string>();
            std::string parentId = ganttData[i]["parentId"].as<std::string>();
            float delayDays = ganttData[i]["delayDays"].asFloat();

            for (int j = i + 1; j < ganttData.size(); ++j)
            {
                if (ganttData[j]["parentId"].as<std::string>() == parentId)
                {
                    std::string originalStart = ganttData[j]["start"].as<std::string>();
                    std::string originalEnd = ganttData[j]["end"].as<std::string>();

                    std::string newStart = date_utils::addWorkdays(originalStart, delayDays, 1.0);
                    std::string newEnd = date_utils::addWorkdays(originalEnd, delayDays, 1.0);
                    // newGanttData[j]["title"] = newGanttData[j]["title"].as<std::string>() + " (落後" + std::to_string(static_cast<int>(delayDays)) + "天)";
                    // ganttData[j]["title"] = ganttData[j]["title"].as<std::string>() + " (推遲" + std::to_string(static_cast<int>(delayDays)) + "天)";
                    ganttData[j]["title"] = ganttData[j]["title"].as<std::string>();
                    ganttData[j]["start"] = newStart;
                    ganttData[j]["end"] = newEnd;
                    ganttData[j]["delayed"] = true;

                    // LOG_DEBUG << "Adjusted task " << newGanttData[j]["id"].as<std::string>()
                    //           << " start from " << originalStart << " to " << newStart
                    //           << ", end from " << originalEnd << " to " << newEnd;
                }
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