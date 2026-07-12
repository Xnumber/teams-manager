#include <chrono>
#include <iomanip>
#include <sstream>

#include "GanttChartCtrl.h"
#include "sql/sql.h"
#include "../../utils/date/date.h"
#include "GetCorrected/utils.h"
using namespace drogon;
using namespace drogon::orm;

/**
 * 經過延遲、超前、executorTimeRatio修正的甘特圖資料
 */
void GanttChartCtrl::getCorrectedGanttChartData(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{

    // Your implementation here
    LOG_DEBUG << "GetCorrectedGanttChartData called";
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

        addProjectsToGanttData(projectResult, ganttData);
        addMilestonesToGanttData(milestoneResult, ganttData);

        std::string todayDate = date_utils::getTodayDate();
        // 延遲的任務訊息
        Json::Value delayedTasks(Json::arrayValue);
        // 超前的任務訊息
        Json::Value aheadTasks(Json::arrayValue);
        Json::Value firstTaskItem;

        if (taskResult.size() > 0)
        {
            const drogon::orm::Row &firstTaskRawData = taskResult[0];
            std::string taskStart = (!firstTaskRawData["scheduled_start_date"].isNull() && !firstTaskRawData["scheduled_start_date"].as<std::string>().empty())
                                        ? firstTaskRawData["scheduled_start_date"].as<std::string>()
                                        : date_utils::getTodayDate();
            float estimatedWorkdays = firstTaskRawData["estimated_workdays"].as<float>();
            float executorTimeRatio = firstTaskRawData["excecutor_time_ratio"].as<float>();

            
            std::string endDate;

            if (firstTaskRawData["completion_date"].isNull()) {
                endDate = date_utils::addWorkdays(taskStart, estimatedWorkdays, executorTimeRatio);
            } else {
                endDate = firstTaskRawData["completion_date"].as<std::string>();
            }
            bool isCompleted = firstTaskRawData["completed"].as<bool>();
            firstTaskItem = createBasicTaskGanttItem(firstTaskRawData, taskStart, endDate);
            ganttData.append(firstTaskItem);
        }

        if (taskResult.size() > 1)
        {
            for (size_t i = 1; i < taskResult.size(); ++i)
            {
                // LOG_DEBUG << "Processing task " << taskResult[i]["name"].as<std::string>() << " of " << taskResult.size();
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


        Json::Value dependencyData;
        // bool existTaskStartEarlierThanDependencyEnd = false;

        addDependenciesToGanttData(
            dependenciesResult,
            ganttData,
            taskResult,
            dependencyData);

        try
        {
            Json::Value newGanttData(Json::arrayValue);
            std::string todayDate = date_utils::getTodayDate();

            // newGanttData, 
            processAheadTask(
                ganttData, 
                todayDate,
                aheadTasks
            );

            // processDelayedTask(
            //     ganttData, 
            //     todayDate, 
            //     delayedTasks
            // );

            
            correctDatesByDependencies(
                dependenciesResult,
                ganttData,
                // newGanttData,
                taskResult,
                dependencyData
            );
            
            
            // ganttData = newGanttData;
        }
        catch (const std::exception &e)
        {
            LOG_ERROR << "Error while correcting dates by dependencies: " << e.what();
            Json::Value ret;
            ret["result"] = "error";
            ret["message"] = "校正時間過長，可能有循環依賴、未設定的優先序。";
            ret["dependencies"] = dependencyData;

            ret["latestTaskEnd"] = findLatestTaskEndDate(ganttData);

            if (delayedTasks.size() > 0)
            {
                ret["delayedTasks"] = delayedTasks;
                int totalDelayedDays = 0;

                for (const auto &delayedTask : delayedTasks)
                {
                    totalDelayedDays += delayedTask["delayDays"].asInt();
                }
                ret["totalDelayedDays"] = totalDelayedDays;
            }

            if (aheadTasks.size() > 0)
            {
                ret["aheadTasks"] = aheadTasks;
                int totalAheadDays = 0;

                for (const auto &aheadTask : aheadTasks)
                {
                    totalAheadDays += aheadTask["aheadDays"].asInt();
                }
                ret["totalAheadDays"] = totalAheadDays;
            }

            drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(ret);
            resp->setStatusCode(k500InternalServerError);
            callback(resp);
            return;
        }

        std::string latestTaskEndDate = findLatestTaskEndDate(ganttData);
        if (latestTaskEndDate.empty())
        {
            latestTaskEndDate = todayDate;
        }

        Json::Value ret;
        ret["data"] = ganttData;
        ret["dependencies"] = dependencyData;
        ret["result"] = "ok";
        ret["latestTaskEnd"] = latestTaskEndDate;

        if (delayedTasks.size() > 0)
        {
            ret["delayedTasks"] = delayedTasks;
            int totalDelayedDays = 0;

            for (const auto &delayedTask : delayedTasks)
            {
                totalDelayedDays += delayedTask["delayDays"].asInt();
            }
            ret["totalDelayedDays"] = totalDelayedDays;
        }

        if (aheadTasks.size() > 0)
        {
            ret["aheadTasks"] = aheadTasks;
            int totalAheadDays = 0;

            for (const auto &aheadTask : aheadTasks)
            {
                totalAheadDays += aheadTask["aheadDays"].asInt();
            }
            ret["totalAheadDays"] = totalAheadDays;
        }

        int estimatedRemainingWorkdays = date_utils::daysBetweenWorkDays(
            date_utils::getTodayDate(),
            latestTaskEndDate
        );

        std::string optimisticEstimatedCompleteDate = date_utils::addWorkdays(todayDate, estimatedRemainingWorkdays * 0.5, 1);
        std::string pessimisticEstimatedCompleteDate = date_utils::addWorkdays(todayDate, estimatedRemainingWorkdays * 1.5, 1);

        std::future<drogon::orm::Result> updatePlanEstimationHistories =
            clientPtr->execSqlAsyncFuture(
                updatePlanEstimationHistoriesSql,
                planIdStr,
                todayDate,
                latestTaskEndDate,
                optimisticEstimatedCompleteDate,
                pessimisticEstimatedCompleteDate,
                estimatedRemainingWorkdays
            );
        drogon::orm::Result updatePlanEstimationHistoriesResult = updatePlanEstimationHistories.get();

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