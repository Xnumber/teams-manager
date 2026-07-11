#include "MilestoneCtrl.h"
#include <jsoncpp/json/json.h>
#include <drogon/HttpResponse.h>
#include "models/Milestones.h"
#include "jwt/jwt.hpp"
#include "sql/milestone-reprioritize-sql.h"

using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::teams_manager;

void Milestone::reprioritize(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback)
{
    const std::shared_ptr<Json::Value> json = req->getJsonObject();
    if (!json || !(*json)["priority"].isInt() || !(*json)["milestone_id"].isString() || !(*json)["project_id"].isString())
    {
        Json::Value err;
        err["result"] = "error";
        err["message"] = "Invalid request body. 'priority' must be int, 'milestone_id' and 'project_id' must be string.";
        auto resp = HttpResponse::newHttpJsonResponse(err);
        resp->setStatusCode(k400BadRequest);
        callback(resp);
        return;
    }

    const int newPriority = (*json)["priority"].asInt();
    const std::string projectId = (*json)["project_id"].asString();
    const std::string milestoneId = (*json)["milestone_id"].asString();
    const std::string tenantId = (*json)["tenant_id"].asString();

    if (newPriority < 0)
    {
        Json::Value err;
        err["result"] = "error";
        err["message"] = "'priority' must be >= 0.";
        auto resp = HttpResponse::newHttpJsonResponse(err);
        resp->setStatusCode(k400BadRequest);
        callback(resp);
        return;
    }

    DbClientPtr dbClient = drogon::app().getDbClient("teams_manager");
    auto trans = dbClient->newTransaction();

    trans->execSqlAsync(
        getMilestoneReprioritizeReorderSql(),
        [callback](const Result &r2) {
            Json::Value ret;
            ret["result"] = "ok";
            ret["message"] = "Milestones reprioritized successfully.";
            ret["updated_count"] = static_cast<int>(r2.affectedRows());
            auto resp = HttpResponse::newHttpJsonResponse(ret);
            callback(resp);
        },
        [callback](const DrogonDbException &e) {
            LOG_ERROR << "Milestone reorder error: " << e.base().what();
            Json::Value err;
            err["result"] = "error";
            err["message"] = std::string("Reorder failed: ") + e.base().what();
            auto resp = HttpResponse::newHttpJsonResponse(err);
            resp->setStatusCode(k500InternalServerError);
            callback(resp);
        },
        projectId, milestoneId, newPriority);
}