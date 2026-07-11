#include "GanttChartCtrl.h"
#include "sql/sql.h"
#include "../../utils/date/date.h"
#include <chrono>
#include <iomanip>
#include <sstream>
using namespace drogon;
using namespace drogon::orm;



// Add definition of your processing function here
/**
 * 不經過executorTimeRatio修正的甘特圖資料
 * 資料也不經過延遲、超前的項目修正
 */
void GanttChartCtrl::getGanttChartData(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{

    // Your implementation here
    LOG_DEBUG << "Teams get called";
    std::string planIdStr = req->getParameter("plan_id");

    if (planIdStr.empty())
    {
        Json::Value error;
        error["result"] = "error";
        error["message"] = "Invalid Plan ID";
        drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(k400BadRequest);
        callback(resp);
        return;
    }

    DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
    // orm::Mapper<Teams> mapper(clientPtr);
    try
    {
        // Teams team = mapper.findByPrimaryKey(teamId);
        std::future<drogon::orm::Result> projectFuture =
            clientPtr->execSqlAsyncFuture(getProjectsSql, planIdStr);

        std::future<drogon::orm::Result> milestoneFuture =
            clientPtr->execSqlAsyncFuture(getMilestonesSql, planIdStr);

        std::future<drogon::orm::Result> taskFuture =
            clientPtr->execSqlAsyncFuture(getTasksSql, planIdStr);

        std::future<drogon::orm::Result> dependenciesFuture =
            clientPtr->execSqlAsyncFuture(getDependenciesSql);

        drogon::orm::Result projectResult = projectFuture.get();
        drogon::orm::Result milestoneResult = milestoneFuture.get();
        drogon::orm::Result taskResult = taskFuture.get();
        drogon::orm::Result dependenciesResult = dependenciesFuture.get();

        Json::Value ganttData(Json::arrayValue);

        for (const drogon::orm::Row &project : projectResult)
        {
            Json::Value item;
            item["id"] = project["id"].as<std::string>();
            item["parentId"] = 0;
            item["title"] = project["name"].as<std::string>();
            item["workItemType"] = "PROJECT";
            item["priority"] = project["priority"].as<int>();
            ganttData.append(item);
        }

        for (const drogon::orm::Row &milestone : milestoneResult)
        {
            Json::Value item;
            item["id"] = milestone["id"].as<std::string>();
            item["parentId"] = milestone["project_id"].as<std::string>();
            item["title"] = milestone["name"].as<std::string>();
            item["workItemType"] = "MILESTONE";
            item["priority"] = milestone["priority"].as<int>();
            ganttData.append(item);
        }

        // const Json::Value &tasks = Json::arrayValue;

        std::string todayDate = date_utils::getTodayDate();
        // 延遲的任務訊息
        Json::Value delayedTasks(Json::arrayValue);
        // 超前的任務訊息
        Json::Value aheadTasks(Json::arrayValue);

        // bool adjustedOnce = false;
        // int delayDays = 0;
        // std::string delayTaskName = "";
        // std::string delayedTaskId;
        Json::Value firstTaskItem;

        if (taskResult.size() > 0)
        {
            const drogon::orm::Row &firstTaskRawData = taskResult[0];
            if (!firstTaskRawData["scheduled_start_date"].isNull() && !firstTaskRawData["scheduled_start_date"].as<std::string>().empty())
            {
                firstTaskItem["start"] = firstTaskRawData["scheduled_start_date"].as<std::string>();
            }
            else
            {
                firstTaskItem["start"] = date_utils::getTodayDate();
            }

            float estimatedWorkdays = firstTaskRawData["estimated_workdays"].as<float>();
            // float executorTimeRatio = firstTaskRawData["excecutor_time_ratio"].as<float>();
            // LOG_DEBUG << "Estimated workdays: " << estimatedWorkdays;
            // LOG_DEBUG << "Executor time ratio: " << executorTimeRatio;
            // LOG_DEBUG << "Start date: " << firstTaskItem["start"].as<std::string>();

            std::string endDate = date_utils::addWorkdays(
                firstTaskItem["start"].as<std::string>(),
                estimatedWorkdays,
                1
            );

            bool isCompleted = firstTaskRawData["completed"].as<bool>();

            
            
            
            if(isCompleted && endDate > todayDate)
            {
                // 收集超前任務訊息
                std::string aheadTaskName;
                float aheadDays = 0;
                std::string aheadTaskId;
                std::string originalTaskEnd = endDate;
                aheadTaskName = firstTaskRawData["name"].as<std::string>();
                aheadTaskId = firstTaskRawData["id"].as<std::string>();

                int daysDiff = date_utils::daysBetweenWorkDays(todayDate, originalTaskEnd);

                if (daysDiff > estimatedWorkdays)
                {
                    aheadDays = estimatedWorkdays;
                }
                else
                {
                    aheadDays = daysDiff;
                }

                Json::Value aheadTask;

                aheadTask["id"] = firstTaskRawData["id"].as<std::string>();
                aheadTask["name"] = firstTaskRawData["name"].as<std::string>();
                aheadTask["scheduled_end_date"] = originalTaskEnd;
                aheadTask["aheadDays"] = aheadDays;
                aheadTasks.append(aheadTask);

                LOG_DEBUG << "Task " << aheadTaskId
                          << " completed early, scheduled end: " << originalTaskEnd
                          << ", today: " << todayDate
                          << " (difference: " << daysDiff << " days)";
                firstTaskItem["ahead"] = true;
            }


            if (!isCompleted && endDate < todayDate)
            {
                // 收集延遲任務訊息
                std::string delayTaskName;
                int delayDays = 0;
                std::string delayedTaskId;
                std::string originalTaskEnd = endDate;
                delayTaskName = firstTaskRawData["name"].as<std::string>();
                delayedTaskId = firstTaskRawData["id"].as<std::string>();

                int daysDiff = date_utils::daysBetweenWorkDays(originalTaskEnd, todayDate);

                delayDays = daysDiff;

                Json::Value delayedTask;

                delayedTask["id"] = firstTaskRawData["id"].as<std::string>();
                delayedTask["name"] = firstTaskRawData["name"].as<std::string>();
                delayedTask["scheduled_end_date"] = originalTaskEnd;
                delayedTask["delayDays"] = daysDiff;
                delayedTasks.append(delayedTask);

                // LOG_DEBUG << "Task " << delayedTaskId
                //           << " adjusted from " << originalTaskEnd
                //           << " to " << todayDate
                //           << " (difference: " << daysDiff << " days)";
                firstTaskItem["delayed"] = true;
            }

            firstTaskItem["end"] = endDate;

            firstTaskItem["id"] = firstTaskRawData["id"].as<std::string>();
            firstTaskItem["parentId"] = firstTaskRawData["milestone_id"].as<std::string>();
            firstTaskItem["title"] = firstTaskRawData["name"].as<std::string>();
            firstTaskItem["workItemType"] = "TASK";
            firstTaskItem["priority"] = firstTaskRawData["priority"].as<std::string>();
            firstTaskItem["completed"] = firstTaskRawData["completed"].as<bool>();
            firstTaskItem["progress"] = firstTaskRawData["progress"].as<float>();
            ganttData.append(firstTaskItem);
        }

        // Add remaining tasks
        std::string latestTaskEnd;

        if (taskResult.size() > 1)
        {
            for (size_t i = 1; i < taskResult.size(); ++i)
            {
                // LOG_DEBUG << "Processing task " << taskResult[i]["name"].as<std::string>() << " of " << taskResult.size();
                const drogon::orm::Row &taskRawData = taskResult[i];
                Json::Value taskItem;
                std::string taskStart;
                if (!taskRawData["scheduled_start_date"].isNull() && !taskRawData["scheduled_start_date"].as<std::string>().empty())
                {
                    taskStart = taskRawData["scheduled_start_date"].as<std::string>();
                }
                else
                {
                    std::string previousTaskEnd = ganttData[ganttData.size() - 1]["end"].as<std::string>();
                    taskStart = date_utils::getNextDate(previousTaskEnd);
                    // LOG_DEBUG << "Task "  << " previous end: " << previousTaskEnd;
                    // LOG_DEBUG << "Task "  << " start date: " << taskStart;
                }
                taskItem["start"] = taskStart;

                float estimatedWorkdays = taskRawData["estimated_workdays"].as<float>();
                // float executorTimeRatio = taskRawData["excecutor_time_ratio"].as<float>();

                std::string taskEnd = date_utils::addWorkdays(
                    taskStart,
                    estimatedWorkdays,
                    1
                );

                // Adjust taskEnd to today if task is incomplete and taskEnd is before today (only once)
                bool isCompleted = taskRawData["completed"].as<bool>();


                if(isCompleted && taskEnd > todayDate)
                {
                    // 收集超前任務訊息
                    std::string aheadTaskName;
                    float aheadDays = 0;
                    std::string aheadTaskId;
                    std::string originalTaskEnd = taskEnd;
                    aheadTaskName = taskRawData["name"].as<std::string>();
                    aheadTaskId = taskRawData["id"].as<std::string>();

                    int daysDiff = date_utils::daysBetweenWorkDays(todayDate, originalTaskEnd);

                    
                    if (daysDiff > estimatedWorkdays)
                    {
                        aheadDays = estimatedWorkdays;
                    }
                    else
                    {
                        aheadDays = daysDiff;
                    }

                    Json::Value aheadTask;

                    aheadTask["id"] = taskRawData["id"].as<std::string>();
                    aheadTask["name"] = taskRawData["name"].as<std::string>();
                    aheadTask["scheduled_end_date"] = originalTaskEnd;
                    aheadTask["aheadDays"] = aheadDays;
                    aheadTasks.append(aheadTask);

                    LOG_DEBUG << "Task " << aheadTaskId
                            << " completed early, scheduled end: " << originalTaskEnd
                            << ", today: " << todayDate
                            << " (difference: " << daysDiff << " days)";
                    taskItem["ahead"] = true;
                }


                if (!isCompleted && taskEnd < todayDate)
                {
                    // 收集延遲任務訊息
                    std::string delayTaskName;
                    int delayDays = 0;
                    std::string delayedTaskId;
                    std::string originalTaskEnd = taskEnd;
                    delayTaskName = taskRawData["name"].as<std::string>();
                    delayedTaskId = taskRawData["id"].as<std::string>();

                    int daysDiff = date_utils::daysBetweenWorkDays(originalTaskEnd, todayDate);

                    delayDays = daysDiff;

                    Json::Value delayedTask;

                    delayedTask["id"] = taskRawData["id"].as<std::string>();
                    delayedTask["name"] = taskRawData["name"].as<std::string>();
                    delayedTask["scheduled_end_date"] = originalTaskEnd;
                    delayedTask["delayDays"] = daysDiff;
                    delayedTasks.append(delayedTask);

                    // LOG_DEBUG << "Task " << delayedTaskId
                    //           << " adjusted from " << originalTaskEnd
                    //           << " to " << todayDate
                    //           << " (difference: " << daysDiff << " days)";

                    taskItem["delayed"] = true;
                }

                taskItem["end"] = taskEnd;

                if (latestTaskEnd.empty() || taskEnd > latestTaskEnd)
                {
                    latestTaskEnd = taskEnd;
                }

                taskItem["id"] = taskRawData["id"].as<std::string>();
                taskItem["parentId"] = taskRawData["milestone_id"].as<std::string>();
                taskItem["title"] = taskRawData["name"].as<std::string>();
                taskItem["workItemType"] = "TASK";
                taskItem["priority"] = taskRawData["priority"].as<int>();
                taskItem["completed"] = taskRawData["completed"].as<bool>();
                taskItem["progress"] = taskRawData["progress"].as<float>();

                ganttData.append(taskItem);
            }
        }

        if (!latestTaskEnd.empty())
        {
            // LOG_DEBUG << "Latest task end date: " << latestTaskEnd;
        }

        Json::Value dependencyData;

        for (const drogon::orm::Row &dependency : dependenciesResult)
        {
            std::string predecessorId = dependency["predecessor_id"].as<std::string>();
            std::string successorId = dependency["successor_id"].as<std::string>();
            Json::Value dependencyItem;
            dependencyItem["id"] = dependency["id"].as<std::string>();
            dependencyItem["predecessorId"] = predecessorId;
            dependencyItem["predecessorType"] = dependency["predecessor_type"].as<std::string>();
            dependencyItem["successorId"] = successorId;
            dependencyItem["successorType"] = dependency["successor_type"].as<std::string>();
            dependencyData.append(dependencyItem);
        }

        Json::Value ret;
        ret["data"] = ganttData;
        ret["dependencies"] = dependencyData;


        ret["latestTaskEnd"] = latestTaskEnd;

        if (delayedTasks.size() > 0)
        {
            ret["delayedTasks"] = delayedTasks;
        }

        if (aheadTasks.size() > 0)
        {
            ret["aheadTasks"] = aheadTasks;
        }

        drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(ret);
        callback(resp);
    }
    catch (const std::exception &e)
    {
        LOG_ERROR << "Error: " << e.what();
        Json::Value error;
        error["result"] = "error";
        error["message"] = e.what();
        drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(k500InternalServerError);
        callback(resp);
    }
}