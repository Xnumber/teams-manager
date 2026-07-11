#include "TaskCtrl.h"
#include <drogon/HttpResponse.h>
#include <drogon/orm/Exception.h>
#include "plugins/RequestContext/RequestContext.h"
#include "sql/get-task-statistics.h"


using namespace drogon;
void TaskCtrl::statistics(const HttpRequestPtr &req,
                 std::function<void(const HttpResponsePtr &)> &&callback)
{
    std::string teamId = req->getParameter("team_id");
    std::string month = req->getParameter("month");

    if (teamId.empty())
    {
        Json::Value error;
        error["result"] = "error";
        error["message"] = "team_id is required";
        auto resp = HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(k400BadRequest);
        callback(resp);
        return;
    }
    
    if (month.empty())
    {
        // 預設為當月
        auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        std::tm now_tm;
        localtime_r(&now_c, &now_tm);
        char buffer[8];
        std::strftime(buffer, sizeof(buffer), "%Y-%m", &now_tm);
        month = buffer;
    }

    // Normalize to first day of month for SQL month filtering.
    if (month.size() == 7)
    {
        month += "-01";
    }

    try
    {
        auto clientPtr = drogon::app().getDbClient("teams_manager");
        drogon::orm::Result result = clientPtr->execSqlSync(getTaskStatisticsSql, teamId, month);

        Json::Value rows(Json::arrayValue);
        for (const auto &row : result)
        {
            Json::Value item;
            item["username"] = row["username"].as<std::string>();
            item["tasks_count"] = Json::Value(static_cast<Json::Int64>(row["tasks_count"].as<long long>()));
            item["participated_projects_count"] = Json::Value(static_cast<Json::Int64>(row["participated_projects_count"].as<long long>()));
            rows.append(item);
        }

        Json::Value json;
        json["result"] = "ok";
        json["month"] = month;
        json["data"] = rows;
        json["count"] = static_cast<int>(rows.size());

        auto resp = HttpResponse::newHttpJsonResponse(json);
        callback(resp);
    }
    catch (const drogon::orm::DrogonDbException &e)
    {
        Json::Value error;
        error["result"] = "error";
        error["message"] = e.base().what();
        auto resp = HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(k500InternalServerError);
        callback(resp);
    }
}