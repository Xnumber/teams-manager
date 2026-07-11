#include "TaskCtrl.h"
#include <drogon/HttpResponse.h>
#include "plugins/RequestContext/RequestContext.h"
#include "models/Tasks.h"
#include "sql/list-user-completed-by-month.h"


using namespace drogon;
using namespace drogon::orm;

// using namespace drogon_model::teams_manager;
void TaskCtrl::getCompletedTasksByMonthAndUser(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback)
{
    LOG_DEBUG << "Task getCompletedTasksByMonthAndUser called";
    DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
    try
    {
        std::string userId = req->getParameter("user_id");
        if (userId.empty())
        {
            userId = RequestContext::getUserId();
        }
        // std::string userId = RequestContext::getUserId();

        if (userId.empty())
        {
            Json::Value ret;
            ret["result"] = "ok";
            ret["count"] = 0;
            auto resp = HttpResponse::newHttpJsonResponse(ret);
            callback(resp);
            return;
        }

        std::string month = req->getParameter("month");

        if (month.empty())
        {
            Json::Value error;
            error["result"] = "error";
            error["message"] = "month is required";
            auto resp = HttpResponse::newHttpJsonResponse(error);
            resp->setStatusCode(k400BadRequest);
            callback(resp);
            return;
        }

        // if (month.size() == 7)
        // {
        //     month += "-01";
        // }

        drogon::orm::Result result =
            clientPtr->execSqlSync(listUserCompletedTasksByMonthSql,
                                   userId,
                                   month);

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
                    Json::Value parsed;
                    Json::Reader reader;
                    reader.parse(field.as<std::string>(), parsed);
                    taskJson[colName] = parsed;
                }
                else if (
                    colName == "progress" ||
                    colName == "estimated_workdays" || 
                    colName == "excecutor_time_ratio"
                ) {
                    try {
                        double val = std::stod(field.as<std::string>());
                        taskJson[colName] = val;
                    } catch (...) {
                        taskJson[colName] = 0.0;  // 失敗時預設值
                    }
                } else if (colName == "completed" || colName == "delayed")
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
        ret["data"] = tasksJson;
        ret["count"] = static_cast<int>(tasksJson.size());
        drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(ret);
        callback(resp);
    } catch (const std::exception &e)
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