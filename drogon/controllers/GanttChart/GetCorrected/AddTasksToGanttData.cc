#include "utils.h"
#include "../../../utils/date/date.h"



void addTasksToGanttData(
    const drogon::orm::Result &taskResult,
    const std::string &todayDate,
    Json::Value &ganttData
)
{
    if (taskResult.size() > 0)
    {
        const drogon::orm::Row &firstTaskRawData = taskResult[0];
        std::string taskStart = (!firstTaskRawData["scheduled_start_date"].isNull() && !firstTaskRawData["scheduled_start_date"].as<std::string>().empty())
                                    ? firstTaskRawData["scheduled_start_date"].as<std::string>()
                                    : date_utils::getTodayDate();
        float estimatedWorkdays = firstTaskRawData["estimated_workdays"].as<float>();
        float executorTimeRatio = firstTaskRawData["excecutor_time_ratio"].as<float>();

        std::string endDate;

        if (firstTaskRawData["completion_date"].isNull())
        {
            endDate = date_utils::addWorkdays(taskStart, estimatedWorkdays, executorTimeRatio);
        }
        else
        {
            endDate = firstTaskRawData["completion_date"].as<std::string>();
        }

        Json::Value firstTaskItem = createBasicTaskGanttItem(firstTaskRawData, taskStart, endDate);
        ganttData.append(firstTaskItem);
    }

    if (taskResult.size() > 1)
    {
        for (size_t i = 1; i < taskResult.size(); ++i)
        {
            const drogon::orm::Row &taskRawData = taskResult[i];

            std::string taskStart = (!taskRawData["scheduled_start_date"].isNull() && !taskRawData["scheduled_start_date"].as<std::string>().empty())
                                        ? taskRawData["scheduled_start_date"].as<std::string>()
                                        : date_utils::getNextDate(ganttData[ganttData.size() - 1]["end"].as<std::string>());

            processTask(
                taskRawData,
                taskStart,
                todayDate,
                ganttData
            );
        }
    }
}