#include "PlanHistoryCtrl.h"
using namespace drogon;
using namespace drogon::orm;
// using namespace drogon_model::teams_manager;
// Add definition of your processing function here


void PlanHistoryCtrl::planHistory(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, const std::string &planIdStr)
{
    // Your implementation here
    LOG_DEBUG << "PlanHistoryCtrl called with planId: " << planIdStr;

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

    try
    {
        std::future<drogon::orm::Result> historyFuture =
            clientPtr->execSqlAsyncFuture(R"(
                SELECT *
                FROM plan_estimation_histories
                WHERE plan_id = $1
            )", planIdStr);

        drogon::orm::Result historyResult = historyFuture.get();

        Json::Value historyData(Json::arrayValue);

        for (const auto &row : historyResult)
        {
            Json::Value entry;
            entry["estimation_date"] = row["estimation_date"].as<std::string>();
            entry["complete_date"] = row["complete_date"].as<std::string>();
            entry["optimistic_estimated_complete_date"] = row["optimistic_estimated_complete_date"].as<std::string>();
            entry["pessimistic_estimated_complete_date"] = row["pessimistic_estimated_complete_date"].as<std::string>();
            entry["estimated_remaining_workdays"] = row["estimated_remaining_workdays"].as<int>();
            historyData.append(entry);
        }

        Json::Value ret;
        ret["result"] = "ok";
        ret["count"] = historyData.size();
        ret["data"] = historyData;

        drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(ret);
        callback(resp);
    }
    catch (const std::exception &e)
    {
        LOG_ERROR << "Error fetching plan estimation histories: " << e.what();
        Json::Value error;
        error["result"] = "error";
        error["message"] = "Failed to fetch plan estimation histories";
        drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(k500InternalServerError);
        callback(resp);
    }
}