#include "utils.h"
#include "../../../utils/date/date.h"
#include <drogon/utils/Utilities.h>



void processTask(const drogon::orm::Row &taskRawData, const std::string &taskStart, const std::string &todayDate, Json::Value &ganttData)
{
    float estimatedWorkdays = taskRawData["estimated_workdays"].as<float>();
    float executorTimeRatio = taskRawData["excecutor_time_ratio"].as<float>();
    
    std::string endDate;;

    if (taskRawData["completion_date"].isNull() && taskRawData["completed"].as<bool>() == false) {
        endDate = date_utils::addWorkdays(taskStart, estimatedWorkdays, executorTimeRatio);
    } else if (!taskRawData["completion_date"].isNull() && taskRawData["completed"].as<bool>() == true) {
        endDate = taskRawData["completion_date"].as<std::string>();
    } else {
        endDate = date_utils::addWorkdays(taskStart, estimatedWorkdays, executorTimeRatio);
    }


    Json::Value taskItem = createBasicTaskGanttItem(taskRawData, taskStart, endDate);

    // processTaskStatus(taskRawData, endDate, todayDate, estimatedWorkdays, taskItem, ganttData, aheadTasks, delayedTasks);

    // if (latestTaskEnd.empty() || endDate > latestTaskEnd)
    // {
    //     latestTaskEnd = taskEnd;
    // }
    ganttData.append(taskItem);
}