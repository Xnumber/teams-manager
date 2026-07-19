#include "PlanCtrl.h"

#include <drogon/HttpResponse.h>
#include <jsoncpp/json/json.h>

#include "sql/sql.h"

using namespace drogon;
using namespace drogon::orm;

void PlanCtrl::listMetricsHistories(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback,
    std::string planId)
{
    (void)req;
    LOG_DEBUG << "Plan listMetricsHistories called";

    Json::Value ret;

    if (planId.empty())
    {
        ret["result"] = "error";
        ret["message"] = "Invalid plan id";
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(k400BadRequest);
        callback(resp);
        return;
    }

    try
    {
        DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
        Result result = clientPtr->execSqlSync(listMetricsHistoriesSql, planId);

        Json::Value data(Json::arrayValue);
        for (const auto &row : result)
        {
            Json::Value item;
            item["plan_id"] = row["plan_id"].as<std::string>();
            item["date"] = row["date"].as<std::string>();
            item["task_count"] = row["task_count"].as<int>();
            item["task_added_count"] = row["task_added_count"].as<int>();
            item["task_completed_count"] = row["task_completed_count"].as<int>();
            item["milestone_count"] = row["milestone_count"].as<int>();
            item["milestone_added_count"] = row["milestone_added_count"].as<int>();
            item["milestone_completed_count"] = row["milestone_completed_count"].as<int>();
            item["changed_at"] = row["changed_at"].as<std::string>();
            data.append(item);
        }

        ret["result"] = "ok";
        ret["count"] = static_cast<Json::UInt64>(data.size());
        ret["data"] = data;

        auto resp = HttpResponse::newHttpJsonResponse(ret);
        callback(resp);
    }
    catch (const std::exception &e)
    {
        LOG_ERROR << "Error: " << e.what();
        ret["result"] = "error";
        ret["message"] = e.what();
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(k500InternalServerError);
        callback(resp);
    }
}
