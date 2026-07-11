#include "TaskCtrl.h"
#include <drogon/HttpResponse.h>
#include "plugins/RequestContext/RequestContext.h"
#include "sql/get-participating-project-task-stats.h"
#include "models/ExecutorsTasks.h"
#include "models/MentorsTasks.h"
#include "models/Tasks.h"
#include "utils/week_time.h"


using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::teams_manager;
void TaskCtrl::completedTasksThisMonthByUser(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback)
{
    LOG_DEBUG << "Task completedTasksThisMonthByUser called";
    try
    {
        Json::Value tasksJson(Json::arrayValue);
        // 取得 query param
        std::string executorId = req->getParameter("executor_id");
        std::string mentorId = req->getParameter("mentor_id");
        if (executorId.empty() || mentorId.empty())
        {
            Json::Value ret;
            ret["result"] = "ok";
            ret["tasks"] = tasksJson;
            ret["count"] = 0;
            auto resp = HttpResponse::newHttpJsonResponse(ret);
            callback(resp);
            return;
        }
        DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
        orm::Mapper<Tasks> tasksMapper(clientPtr);
        orm::Mapper<MentorsTasks> mentorTaskMapper(clientPtr);
        orm::Mapper<ExecutorsTasks> executorTaskMapper(clientPtr);

        LOG_DEBUG << "Query params - executor_id: " << executorId << ", mentor_id: " << mentorId;

        std::vector<std::string> taskIds;
        std::vector<MentorsTasks> mentorTasks;
        std::vector<ExecutorsTasks> executorTasks;

        // 依據 executor_id 或 mentor_id 查詢中介表
        if (!executorId.empty())
        {
            executorTasks = executorTaskMapper.findBy(Criteria(ExecutorsTasks::Cols::_user_id, CompareOperator::EQ, executorId));
            for (const auto &et : executorTasks)
            {
                taskIds.push_back(et.getValueOfTaskId());
            }
        }

        if (!mentorId.empty())
        {
            mentorTasks = mentorTaskMapper.findBy(Criteria(MentorsTasks::Cols::_user_id, CompareOperator::EQ, mentorId));
            for (const auto &mt : mentorTasks)
            {
                taskIds.push_back(mt.getValueOfTaskId());
            }
        }

        // 去除重複的 task_id
        std::sort(taskIds.begin(), taskIds.end());
        taskIds.erase(std::unique(taskIds.begin(), taskIds.end()), taskIds.end());

        std::vector<Tasks> tasksList;
        if (taskIds.empty())
        {
            Json::Value ret;
            ret["result"] = "ok";
            ret["tasks"] = tasksJson;
            ret["count"] = 0;
            auto resp = HttpResponse::newHttpJsonResponse(ret);
            callback(resp);
            return;
        }
        // 查詢 taskIds 內、已完成、且完成日期在本月內的 tasks
        Criteria criteria = Criteria(Tasks::Cols::_id, CompareOperator::In, taskIds) && Criteria(Tasks::Cols::_completed, CompareOperator::EQ, true) && Criteria(Tasks::Cols::_completion_date, CompareOperator::GE, WeekTimeUtils::getThisMonthStart()) && Criteria(Tasks::Cols::_completion_date, CompareOperator::LE, WeekTimeUtils::getThisMonthEnd());
        tasksList = tasksMapper.findBy(criteria);

        // 取得所有 mentors/executors 關聯
        std::vector<std::string> allTaskIds;
        for (const auto &task : tasksList)
        {
            allTaskIds.push_back(task.getValueOfId());
        }
        // 修正：allTaskIds 為空時直接回傳結果，避免 drogon::orm::Criteria In assertion fail
        if (allTaskIds.empty())
        {
            Json::Value ret;
            ret["result"] = "ok";
            ret["tasks"] = tasksJson;
            ret["count"] = 0;
            auto resp = HttpResponse::newHttpJsonResponse(ret);
            callback(resp);
            return;
        }
        std::vector<MentorsTasks> allMentors = mentorTaskMapper.findBy(Criteria(MentorsTasks::Cols::_task_id, CompareOperator::In, allTaskIds));
        std::vector<ExecutorsTasks> allExecutors = executorTaskMapper.findBy(Criteria(ExecutorsTasks::Cols::_task_id, CompareOperator::In, allTaskIds));

        // 組合回傳 JSON
        for (const auto &task : tasksList)
        {
            Json::Value taskJson = task.toJson();
            taskJson["mentors"] = Json::arrayValue;
            taskJson["executors"] = Json::arrayValue;
            for (const auto &mt : allMentors)
            {
                if (mt.getValueOfTaskId() == task.getValueOfId())
                {
                    taskJson["mentors"].append(mt.toJson());
                }
            }
            for (const auto &et : allExecutors)
            {
                if (et.getValueOfTaskId() == task.getValueOfId())
                {
                    taskJson["executors"].append(et.toJson());
                }
            }
            tasksJson.append(taskJson);
        }

        Json::Value ret;
        ret["result"] = "ok";
        ret["tasks"] = tasksJson;
        ret["count"] = static_cast<int>(tasksJson.size());
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        callback(resp);
    }
    catch (const DrogonDbException &e)
    {
        LOG_ERROR << "Database error: " << e.base().what();
        Json::Value error;
        error["result"] = "error";
        error["message"] = e.base().what();
        auto resp = HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(k500InternalServerError);
        callback(resp);
    }
}