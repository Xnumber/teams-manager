#include "TaskCtrl.h"
#include <jsoncpp/json/json.h>
#include <drogon/HttpResponse.h>
#include "models/Tasks.h"
#include "models/Users.h"
#include "models/MentorsTasks.h"
#include "models/ExecutorsTasks.h"
#include "models/Projects.h"
#include "models/TaskTypes.h"
#include "models/Tenants.h"
#include "models/Teams.h"
#include "../tenant_example/solution1_wrapper/TenantMapper.h"
#include "models/TaskStatus.h"
#include "plugins/RequestContext/RequestContext.h"
#include "sql/list.h"
#include "sql/sql.h"
using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::teams_manager;

#include "utils/mapToJson.h"
#include "utils/week_time.h"
#include "sql/get-participating-project-task-stats-this-month.h"

void TaskCtrl::get(const HttpRequestPtr &req,
                   std::function<void(const HttpResponsePtr &)> &&callback, std::string taskId)
{
    LOG_DEBUG << "Task get called: " << taskId;
    DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
    orm::Mapper<Tasks> mapper(clientPtr);
    try
    {
        Tasks task = mapper.findByPrimaryKey(taskId);
        Json::Value ret;
        ret["result"] = "ok";
        ret["data"] = task.toJson();
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        callback(resp);
    }
    catch (const std::exception &e)
    {
        LOG_ERROR << "Error: " << e.what();
        Json::Value error;
        error["result"] = "error";
        error["message"] = e.what();
        auto resp = HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(k500InternalServerError);
        callback(resp);
    }
}

void TaskCtrl::list(const HttpRequestPtr &req,
                    std::function<void(const HttpResponsePtr &)> &&callback)
{
    LOG_DEBUG << "Task list called";
    try
    {
        DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");

        std::string teamId = RequestContext::getTeamId();

        std::string description = &req->getParameter("description") ? req->getParameter("description") : "";
        std::string filter = &req->getParameter("filter") ? req->getParameter("filter") : "";
        // LOG_DEBUG << "Filtering tasks with description containing: " << description;
        drogon::orm::Result result = clientPtr->execSqlSync(listSql, filter);

        Json::Value tasksJson(Json::arrayValue);

        for (const drogon::orm::Row &row : result)
        {
            Json::Value taskJson;
            for (size_t i = 0; i < row.size(); i++)
            {
                const auto &field = row[i];
                const std::string colName = result.columnName(i);
                if (colName == "mentors" || colName == "executors")
                {
                    // JSON 字串需要解析
                    Json::Value parsed;
                    Json::Reader reader;
                    reader.parse(field.as<std::string>(), parsed);
                    taskJson[colName] = parsed;
                }
                else if (field.isNull())
                {
                    taskJson[colName] = Json::Value::null;
                }

                // 數值型欄位做特殊處理
                else if (colName == "progress" || colName == "excecutor_time_ratio")
                {
                    try
                    {
                        double val = std::stod(field.as<std::string>());
                        taskJson[colName] = val;
                    }
                    catch (...)
                    {
                        taskJson[colName] = 0.0;
                    }
                }
                else if (colName == "priority" || colName == "estimated_workdays")
                {
                    try
                    {
                        taskJson[colName] = std::stoi(field.as<std::string>());
                    }
                    catch (...)
                    {
                        taskJson[colName] = 0;
                    }
                }
                else if (colName == "completed" || colName == "delayed")
                {
                    // BOOLEAN 類型 (PostgreSQL 的 f/t)
                    std::string val = field.as<std::string>();
                    taskJson[colName] = (val == "t" || val == "true" || val == "1");
                }
                else
                {
                    taskJson[colName] = field.as<std::string>();
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

void TaskCtrl::historyTasks(const HttpRequestPtr &req,
                            std::function<void(const HttpResponsePtr &)> &&callback)
{
    LOG_DEBUG << "Task historyTasks called";
    try
    {
        DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");

        // std::string userId = RequestContext::getUserId();
        std::string executorId = req->getParameter("executor_id");
        std::string description = req->getParameter("description");
        std::string name = req->getParameter("name");
        std::string project_name = req->getParameter("project_name");
        std::optional<std::string> milestoneParam;
        std::string milestoneId = req->getParameter("milestone_id");
        std::optional<std::string> statusParam;
        std::string status_id = req->getParameter("status_id");

        if (milestoneId.empty())
        {
            milestoneParam = std::nullopt;
        }
        else
        {
            milestoneParam = milestoneId;
        }

        if (status_id.empty())
        {
            statusParam = std::nullopt;
        }
        else
        {
            statusParam = status_id;
        }
        LOG_DEBUG << "Filtering tasks with description containing: " << description;
        std::string thisMonday = WeekTimeUtils::getThisMonday();
        LOG_DEBUG << "Filtering history tasks with thisMonday: " << thisMonday;
        drogon::orm::Result result = clientPtr->execSqlSync(
            historyTasksSql,
            executorId,
            thisMonday,
            name,
            description,
            milestoneParam,
            project_name,
            statusParam);

        Json::Value tasksJson(Json::arrayValue);

        for (const drogon::orm::Row &row : result)
        {
            Json::Value taskJson;
            for (size_t i = 0; i < row.size(); i++)
            {
                const auto &field = row[i];
                const std::string colName = result.columnName(i);
                if (colName == "mentors" || colName == "executors")
                {
                    // JSON 字串需要解析
                    Json::Value parsed;
                    Json::Reader reader;
                    reader.parse(field.as<std::string>(), parsed);
                    taskJson[colName] = parsed;
                }
                else if (field.isNull())
                {
                    taskJson[colName] = Json::Value::null;
                }

                // 數值型欄位做特殊處理
                else if (colName == "progress" || colName == "excecutor_time_ratio")
                {
                    try
                    {
                        double val = std::stod(field.as<std::string>());
                        taskJson[colName] = val;
                    }
                    catch (...)
                    {
                        taskJson[colName] = 0.0;
                    }
                }
                else if (colName == "priority" || colName == "estimated_workdays")
                {
                    try
                    {
                        taskJson[colName] = std::stoi(field.as<std::string>());
                    }
                    catch (...)
                    {
                        taskJson[colName] = 0;
                    }
                }
                else if (colName == "completed" || colName == "delayed")
                {
                    // BOOLEAN 類型 (PostgreSQL 的 f/t)
                    std::string val = field.as<std::string>();
                    taskJson[colName] = (val == "t" || val == "true" || val == "1");
                }
                else
                {
                    taskJson[colName] = field.as<std::string>();
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

void TaskCtrl::completedTasks(const HttpRequestPtr &req,
                              std::function<void(const HttpResponsePtr &)> &&callback)
{
    LOG_DEBUG << "Task completedTasks called";
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

        // 時間條件
        // auto timeCriteria = Criteria(Tasks::Cols::_scheduled_completion_date, CompareOperator::LT, WeekTimeUtils::getThisMonday());

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
            // 若沒給 id，或查不到任何對應的 task_id，直接回傳空陣列
            Json::Value ret;
            ret["result"] = "ok";
            ret["tasks"] = tasksJson;
            ret["count"] = 0;
            auto resp = HttpResponse::newHttpJsonResponse(ret);
            callback(resp);
            return;
        }
        // 查詢 taskIds 內的 tasks
        Criteria criteria = Criteria(Tasks::Cols::_id, CompareOperator::In, taskIds) && Criteria(Tasks::Cols::_completed, CompareOperator::EQ, true);
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

void TaskCtrl::completedThisMonth(const HttpRequestPtr &req,
                                  std::function<void(const HttpResponsePtr &)> &&callback)
{
    LOG_DEBUG << "Task completedThisMonth called";
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

void TaskCtrl::inCompleteTasks(const HttpRequestPtr &req,
                               std::function<void(const HttpResponsePtr &)> &&callback)
{
    LOG_DEBUG << "Task inCompleteTasks called";
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
        // 查詢 taskIds 內且尚未完成的 tasks
        Criteria criteria = Criteria(Tasks::Cols::_id, CompareOperator::In, taskIds) && Criteria(Tasks::Cols::_completed, CompareOperator::EQ, false);
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

// void TaskCtrl::listNextWeek(const HttpRequestPtr &req,
//                             std::function<void(const HttpResponsePtr &)> &&callback)
// {
//     try
//     {
//         // 取得 query param
//         std::string executorId = req->getParameter("executor_id");
//         std::string mentorId = req->getParameter("mentor_id");
//         Json::Value tasksJson(Json::arrayValue);
//         if (executorId.empty() || mentorId.empty())
//         {
//             Json::Value ret;
//             ret["result"] = "ok";
//             ret["tasks"] = tasksJson;
//             ret["count"] = 0;
//             auto resp = HttpResponse::newHttpJsonResponse(ret);
//             callback(resp);
//             return;
//         }
//         DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
//         orm::Mapper<Tasks> tasksMapper(clientPtr);
//         orm::Mapper<MentorsTasks> mentorTaskMapper(clientPtr);
//         orm::Mapper<ExecutorsTasks> executorTaskMapper(clientPtr);

//         // 時間條件
//         auto timeCriteria = Criteria(Tasks::Cols::_scheduled_completion_date, CompareOperator::GT, WeekTimeUtils::getNextMonday()) &&
//                             Criteria(Tasks::Cols::_scheduled_completion_date, CompareOperator::LE, WeekTimeUtils::getNextFriday());

//         std::vector<std::string> taskIds;
//         std::vector<MentorsTasks> mentorTasks;
//         std::vector<ExecutorsTasks> executorTasks;

//         // 依據 executor_id 或 mentor_id 查詢中介表
//         if (!executorId.empty())
//         {
//             executorTasks = executorTaskMapper.findBy(Criteria(ExecutorsTasks::Cols::_user_id, CompareOperator::EQ, executorId));
//             for (const auto &et : executorTasks)
//             {
//                 taskIds.push_back(et.getValueOfTaskId());
//             }
//         }
//         if (!mentorId.empty())
//         {
//             mentorTasks = mentorTaskMapper.findBy(Criteria(MentorsTasks::Cols::_user_id, CompareOperator::EQ, mentorId));
//             for (const auto &mt : mentorTasks)
//             {
//                 taskIds.push_back(mt.getValueOfTaskId());
//             }
//         }

//         // 去除重複的 task_id
//         std::sort(taskIds.begin(), taskIds.end());
//         taskIds.erase(std::unique(taskIds.begin(), taskIds.end()), taskIds.end());

//         std::vector<Tasks> tasksList;
//         if (taskIds.empty())
//         {
//             // 若沒給 id，或查不到任何對應的 task_id，直接回傳空陣列
//             Json::Value ret;
//             ret["result"] = "ok";
//             ret["tasks"] = tasksJson;
//             ret["count"] = 0;
//             auto resp = HttpResponse::newHttpJsonResponse(ret);
//             callback(resp);
//             return;
//         }
//         // 查詢符合時間條件且在 taskIds 內的 tasks
//         auto criteria = timeCriteria && Criteria(Tasks::Cols::_id, CompareOperator::In, taskIds);
//         tasksList = tasksMapper.findBy(criteria);

//         // 取得所有 mentors/executors 關聯
//         std::vector<std::string> allTaskIds;
//         for (const auto &task : tasksList)
//         {
//             allTaskIds.push_back(task.getValueOfId());
//         }
//         if (allTaskIds.empty())
//         {
//             Json::Value ret;
//             ret["result"] = "ok";
//             ret["tasks"] = tasksJson;
//             ret["count"] = 0;
//             auto resp = HttpResponse::newHttpJsonResponse(ret);
//             callback(resp);
//             return;
//         }
//         std::vector<MentorsTasks> allMentors = mentorTaskMapper.findBy(Criteria(MentorsTasks::Cols::_task_id, CompareOperator::In, allTaskIds));
//         std::vector<ExecutorsTasks> allExecutors = executorTaskMapper.findBy(Criteria(ExecutorsTasks::Cols::_task_id, CompareOperator::In, allTaskIds));

//         // 組合回傳 JSON
//         for (const auto &task : tasksList)
//         {
//             Json::Value taskJson = task.toJson();
//             taskJson["mentors"] = Json::arrayValue;
//             taskJson["executors"] = Json::arrayValue;
//             for (const auto &mt : allMentors)
//             {
//                 if (mt.getValueOfTaskId() == task.getValueOfId())
//                 {
//                     taskJson["mentors"].append(mt.toJson());
//                 }
//             }
//             for (const auto &et : allExecutors)
//             {
//                 if (et.getValueOfTaskId() == task.getValueOfId())
//                 {
//                     taskJson["executors"].append(et.toJson());
//                 }
//             }
//             tasksJson.append(taskJson);
//         }

//         Json::Value ret;
//         ret["result"] = "ok";
//         ret["tasks"] = tasksJson;
//         ret["count"] = static_cast<int>(tasksJson.size());
//         auto resp = HttpResponse::newHttpJsonResponse(ret);
//         callback(resp);
//     }
//     catch (const DrogonDbException &e)
//     {
//         LOG_ERROR << "Database error: " << e.base().what();
//         Json::Value error;
//         error["result"] = "error";
//         error["message"] = e.base().what();
//         auto resp = HttpResponse::newHttpJsonResponse(error);
//         resp->setStatusCode(k500InternalServerError);
//         callback(resp);
//     }
// }

// void TaskCtrl::create(const HttpRequestPtr &req,
//                       std::function<void(const HttpResponsePtr &)> &&callback)
// {
//     LOG_DEBUG << "Task create called";
//     DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
//     try
//     {
//         std::shared_ptr<Json::Value> json = req->getJsonObject();
//         if (!json)
//         {
//             Json::Value error;
//             error["result"] = "error";
//             error["message"] = "Invalid JSON body";
//             auto resp = HttpResponse::newHttpJsonResponse(error);
//             resp->setStatusCode(k400BadRequest);
//             callback(resp);
//             return;
//         }
//         json->removeMember("created_at");
//         json->removeMember("completion_date");
//         if (!json->isMember("this_week"))
//         {
//             (*json)["this_week"] = true;
//         }
//         // 開始 transaction
//         std::shared_ptr<drogon::orm::Transaction> trans = clientPtr->newTransaction();
//         orm::Mapper<MentorsTasks> mentorTaskMapper(trans);
//         orm::Mapper<TaskTypes> taskTypesMapper(trans);
//         orm::Mapper<ExecutorsTasks> executorTaskMapper(trans);
//         orm::Mapper<TaskStatus> taskStatusMapper(trans);
//         orm::Mapper<Teams> teamMapper(trans);
//         // orm::Mapper<Tasks> mapper(trans);
//         TenantMapper<Tasks> mapper(trans);
//         // LOG_DEBUG << "Received JSON: " << json->toStyledString();
//         std::string projectId = (*json)["project_id"].asString();

//         std::string task_status_id = (*json)["status_id"].asString();

//         std::string task_status_name = taskStatusMapper.findByPrimaryKey(task_status_id).getValueOfName(); // 預設新工作狀態為 "待處理"，假設其 ID 為 "1"
//         // std::string team_id = (*json)["team_id"].asString();
//         std::string team_id = RequestContext::getTeamId();
//         LOG_DEBUG << "Create Task - Team ID from RequestContext: " << team_id;
//         Teams team = teamMapper.findByPrimaryKey(team_id);
//         std::string team_name = team.getValueOfName();

//         (*json)["status_name"] = task_status_name;
//         (*json)["team_name"] = team_name;
//         (*json)["team_id"] = team_id;

//         // std::string tenantId = (*json)["tenant_id"].asString();

//         if (json->isMember("project_id") && !projectId.empty())
//         {
//             orm::Mapper<Projects> projectsMapper(trans);
//             try
//             {
//                 Projects project = projectsMapper.findByPrimaryKey(projectId);
//                 (*json)["project_name"] = project.getValueOfName();
//             }
//             catch (const std::exception &e)
//             {
//                 LOG_ERROR << "Project fetch error: " << e.what();
//             }
//         }

//         if (json->isMember("task_type_id") && (*json)["task_type_id"].isString())
//         {
//             try
//             {
//                 TaskTypes taskType = taskTypesMapper.findByPrimaryKey((*json)["task_type_id"].asString());
//                 (*json)["task_type_name"] = taskType.getValueOfName();
//             }
//             catch (const std::exception &e)
//             {
//                 LOG_ERROR << "TaskType fetch error: " << e.what();
//             }
//         }
//         // 處理 this_week 自動日期
//         bool thisWeek = (*json)["this_week"].asBool();
//         if (json->isMember("this_week"))
//         {
//             if (thisWeek)
//             {
//                 (*json)["scheduled_start_date"] = WeekTimeUtils::getThisMonday();
//                 (*json)["scheduled_end_date"] = WeekTimeUtils::getThisFriday();
//             }
//             else
//             {
//                 (*json)["scheduled_start_date"] = WeekTimeUtils::getNextMonday();
//                 (*json)["scheduled_end_date"] = WeekTimeUtils::getNextFriday();
//             }
//         }

//         // 若沒有 scheduled_completion_date，根據 this_week 設定
//         if (!json->isMember("scheduled_completion_date") || (*json)["scheduled_completion_date"].isNull())
//         {
//             if (thisWeek)
//             {
//                 (*json)["scheduled_completion_date"] = WeekTimeUtils::getThisFriday();
//             }
//             else
//             {
//                 (*json)["scheduled_completion_date"] = WeekTimeUtils::getNextFriday();
//             }
//         }

//         // LOG_DEBUG << "Final JSON for Task creation: " << json->toStyledString();
//         Tasks newTask(*json);

//         // 將executorIds與mentorIds透過Users找到後集合成一個陣列，轉成到tasks表的participants欄位，但json的物件內有type可以辨別是executor或mentor
//         // std::vector<Json::Value> participants;
//         orm::Mapper<Users> usersMapper(trans);
//         std::string creator_id = newTask.getValueOfCreatorId();
//         std::string creator_name = newTask.getValueOfCreatorName();
//         LOG_DEBUG << "Creator ID: " << creator_id << ", Creator Name: " << creator_name;
//         newTask.setCreatorName(creator_name);
//         newTask.setPriority(0);
//         mapper.insert(newTask);
//         // LOG_DEBUG << "Generated Task ID: " << task_id;
//         // LOG_DEBUG << "Generated Task ID: " << task_id;
//         // LOG_DEBUG << "Generated Task ID: " << task_id;

//         Json::Value executors(Json::arrayValue);
//         Json::Value mentors(Json::arrayValue);

//         if (json->isMember("mentor_ids") && (*json)["mentor_ids"].isArray())
//         {
//             LOG_DEBUG << "OO Mentor ID: ";
//             for (const auto &mentor_id : (*json)["mentor_ids"])
//             {
//                 LOG_DEBUG << "O Mentor ID: " << mentor_id.asString();
//                 if (mentor_id.isString())
//                 {
//                     LOG_DEBUG << "Mentor ID: " << mentor_id.asString();
//                     MentorsTasks mt;
//                     Users user = usersMapper.findByPrimaryKey(mentor_id.asString());
//                     mt.setTaskId(newTask.getValueOfId());
//                     mt.setUserId(mentor_id.asString());
//                     mt.setUserName(user.getValueOfUsername());
//                     mentorTaskMapper.insert(mt);
//                     mentors.append(mt.toJson());
//                 }
//             }
//         }

//         if (json->isMember("executor_ids") && (*json)["executor_ids"].isArray())
//         {
//             LOG_DEBUG << "OO Executor ID: ";
//             for (const auto &executor_id : (*json)["executor_ids"])
//             {
//                 LOG_DEBUG << "O Executor ID: " << executor_id.asString();
//                 if (executor_id.isString())
//                 {
//                     LOG_DEBUG << "Executor ID: " << executor_id.asString();
//                     ExecutorsTasks et;
//                     et.setTaskId(newTask.getValueOfId());
//                     et.setUserId(executor_id.asString());
//                     Users user = usersMapper.findByPrimaryKey(executor_id.asString());
//                     et.setUserName(user.getValueOfUsername());
//                     executorTaskMapper.insert(et);
//                     executors.append(et.toJson());
//                 }
//             }
//         }

//         // std::string creator_id = newTask.getValueOfCreatorId();
//         // std::string creator_name = newTask.getValueOfCreatorName();
//         // LOG_DEBUG << "Creator ID: " << creator_id << ", Creator Name: " << creator_name;
//         // newTask.setCreatorName(creator_name);
//         // mapper.insert(newTask);

//         // trans->rollback(); // 目前先 rollback，等到一切確定沒問題再改成 commit
//         // trans->setCommitCallback();
//         // trans->rollback(); // 目前先 rollback，等到一切確定沒問題再改成 commit

//         Json::Value data = newTask.toJson();
//         data["mentors"] = mentors;
//         data["executors"] = executors;

//         Json::Value ret;
//         ret["result"] = "ok";
//         ret["data"] = data;
//         drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(ret);
//         callback(resp);
//     }
//     catch (const std::exception &e)
//     {
//         LOG_ERROR << "Error: " << e.what();
//         Json::Value error;
//         error["result"] = "error";
//         error["message"] = e.what();
//         auto resp = HttpResponse::newHttpJsonResponse(error);
//         resp->setStatusCode(k500InternalServerError);
//         callback(resp);
//     }
// }


void TaskCtrl::remove(const HttpRequestPtr &req,
                      std::function<void(const HttpResponsePtr &)> &&callback, std::string taskId)
{
    LOG_DEBUG << "Task remove called";
    DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
    orm::Mapper<Tasks> mapper(clientPtr);
    try
    {
        Tasks task = mapper.findByPrimaryKey(taskId);
        mapper.deleteByPrimaryKey(taskId);
        Json::Value ret;
        ret["result"] = "ok";
        ret["task"] = task.toJson();
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        callback(resp);
    }
    catch (const std::exception &e)
    {
        LOG_ERROR << "Error: " << e.what();
        Json::Value error;
        error["result"] = "error";
        error["message"] = e.what();
        auto resp = HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(k500InternalServerError);
        callback(resp);
    }
}

/**
 * 統計本月參與的專案數量（不重複計算同一專案）
 */
void TaskCtrl::getParticipatingProjectsThisMonth(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback)
{
    LOG_DEBUG << "Task getParticipatingProjectsThisMonth called";
    DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
    try
    {
        // 優先使用 query param；若未提供則 fallback 到 request context
        std::string userId = RequestContext::getUserId();
        // if (userId.empty())
        // {
        //     userId = RequestContext::getUserId();
        // }

        if (userId.empty())
        {
            Json::Value ret;
            ret["result"] = "ok";
            ret["count"] = 0;
            ret["projects"] = Json::arrayValue;
            auto resp = HttpResponse::newHttpJsonResponse(ret);
            callback(resp);
            return;
        }

        drogon::orm::Result result =
            clientPtr->execSqlSync(getParticipatingProjectTaskStatsThisMonthSql,
                                   userId);

        Json::Value projects(Json::arrayValue);
        for (const auto &row : result)
        {
            Json::Value item;
            item["project_id"] = row["project_id"].as<std::string>();
            item["project_name"] = row["project_name"].as<std::string>();
            item["task_count"] = row["task_count"].as<int>();
            projects.append(item);
        }

        Json::Value ret;
        ret["result"] = "ok";
        ret["data"] = projects;
        ret["count"] = static_cast<int>(projects.size());
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        callback(resp);
    }
    catch (const std::exception &e)
    {
        LOG_ERROR << "Error: " << e.what();
        Json::Value error;
        error["result"] = "error";
        error["message"] = e.what();
        auto resp = HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(k500InternalServerError);
        callback(resp);
    }
}

// void TaskCtrl::getParticipatingProjectsByMonth(
//     const HttpRequestPtr &req,
//     std::function<void(const HttpResponsePtr &)> &&callback)
// {
//     LOG_DEBUG << "Task getParticipatingProjectsByMonth called";
//     DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
//     try
//     {
//         std::string userId = req->getParameter("user_id");
//         if (userId.empty())
//         {
//             userId = RequestContext::getUserId();
//         }

//         std::string month = req->getParameter("month");

//         if (userId.empty() || month.empty())
//         {
//             Json::Value error;
//             error["result"] = "error";
//             error["message"] = "user_id and month are required";
//             auto resp = HttpResponse::newHttpJsonResponse(error);
//             resp->setStatusCode(k400BadRequest);
//             callback(resp);
//             return;
//         }

//         if (month.size() == 7)
//         {
//             month += "-01";
//         }

//         drogon::orm::Result result =
//             clientPtr->execSqlSync(getParticipatingProjectTaskStatsSql,
//                                    userId,
//                                    month);

//         Json::Value projects(Json::arrayValue);
//         for (const auto &row : result)
//         {
//             Json::Value item;
//             item["project_id"] = row["project_id"].as<std::string>();
//             item["project_name"] = row["project_name"].as<std::string>();
//             item["task_count"] = row["task_count"].as<int>();
//             projects.append(item);
//         }

//         Json::Value ret;
//         ret["result"] = "ok";
//         ret["data"] = projects;
//         ret["count"] = static_cast<int>(projects.size());
//         ret["month"] = month;
//         auto resp = HttpResponse::newHttpJsonResponse(ret);
//         callback(resp);
//     }
//     catch (const std::exception &e)
//     {
//         LOG_ERROR << "Error: " << e.what();
//         Json::Value error;
//         error["result"] = "error";
//         error["message"] = e.what();
//         auto resp = HttpResponse::newHttpJsonResponse(error);
//         resp->setStatusCode(k500InternalServerError);
//         callback(resp);
//     }
// }