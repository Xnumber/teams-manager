#include "TaskCtrl.h"
#include <drogon/HttpResponse.h>
#include "plugins/RequestContext/RequestContext.h"
#include "sql/get-participating-project-task-stats.h"
using namespace drogon;
using namespace drogon::orm;

void TaskCtrl::getParticipatingProjectsByMonth(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback)
{
    LOG_DEBUG << "Task getParticipatingProjectsByMonth called";
    DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
    try
    {
        // std::string userId = req->getParameter("user_id");
        // if (userId.empty())
        // {
        //     userId = RequestContext::getUserId();
        // }
        std::string userId = RequestContext::getUserId();

        std::string month = req->getParameter("month");

        if (userId.empty() || month.empty())
        {
            Json::Value error;
            error["result"] = "error";
            error["message"] = "user_id and month are required";
            auto resp = HttpResponse::newHttpJsonResponse(error);
            resp->setStatusCode(k400BadRequest);
            callback(resp);
            return;
        }

        if (month.size() == 7)
        {
            month += "-01";
        }

        drogon::orm::Result result =
            clientPtr->execSqlSync(getParticipatingProjectTaskStatsSql,
                                   userId,
                                   month);

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
        ret["month"] = month;
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