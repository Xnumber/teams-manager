#include "PlanCtrl.h"
#include <jsoncpp/json/json.h>
#include <drogon/HttpResponse.h>
#include "sql/sql.h"

using namespace drogon;
using namespace drogon::orm;

void PlanCtrl::updateMetricsHistory(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback,
    std::string planId)
{
    LOG_DEBUG << "Plan updateMetricsHistory called";

    DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
    Json::Value ret;

    try
    {
        Result result = clientPtr->execSqlSync(updatePlanMetricsSql, planId);

        if (result.empty())
        {
            ret["result"] = "error";
            ret["message"] = "Failed to update plan metrics history";
            auto resp = HttpResponse::newHttpJsonResponse(ret);
            resp->setStatusCode(k500InternalServerError);
            callback(resp);
            return;
        }

        const auto &row = result[0];
        Json::Value data;
        data["plan_id"] = row["plan_id"].as<std::string>();
        data["date"] = row["date"].as<std::string>();
        data["task_count"] = row["task_count"].as<int>();
        data["task_added_count"] = row["task_added_count"].as<int>();
        data["task_completed_count"] = row["task_completed_count"].as<int>();
        data["milestone_count"] = row["milestone_count"].as<int>();
        data["milestone_added_count"] = row["milestone_added_count"].as<int>();
        data["milestone_completed_count"] = row["milestone_completed_count"].as<int>();
        data["changed_at"] = row["changed_at"].as<std::string>();

        ret["result"] = "ok";
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
