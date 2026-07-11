#include "ProjectMetricsHistory.h"
#include <jsoncpp/json/json.h>
#include <drogon/HttpResponse.h>
#include "models/ProjectMetricsHistory.h"
#include "sql/sql.h"
using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::teams_manager;


void ProjectMetricsHistoryCtrl::updateTaskCountHistory(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback
) {
    LOG_DEBUG << "ProjectMetricsHistory updateTaskCountHistory called";

    DbClientPtr clientPtr =
        drogon::app().getDbClient("teams_manager");
    
        
    try {
        drogon::orm::Result result = clientPtr->execSqlSync(
            updateProjectMetricsHistorySql
            // trantor::Date::now().toFormattedString("%Y-%m-%d")
            // // "2026-06-15"
        );
        Json::Value ret;
        drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(ret);
        ret["result"] = "ok";
        resp->setStatusCode(k200OK);
        callback(HttpResponse::newHttpJsonResponse(ret));
    } catch (const std::exception &e) {
        LOG_ERROR << e.what();
        Json::Value ret;
        ret["result"] = "error";
        ret["message"] = e.what();
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(k500InternalServerError);
        callback(resp);
        return;
    }
}