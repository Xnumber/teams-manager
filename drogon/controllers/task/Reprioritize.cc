#include "TaskCtrl.h"

#include <drogon/HttpResponse.h>
#include <jsoncpp/json/json.h>

#include "jwt/jwt.hpp"
#include "sql/reprioritize-sql.h"

using namespace drogon;
using namespace drogon::orm;

void TaskCtrl::reprioritize(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback)
{
    const std::shared_ptr<Json::Value> json = req->getJsonObject();
    if (!json || !(*json)["priority"].isInt() || !(*json)["task_id"].isString() || !(*json)["milestone_id"].isString())
    {
        Json::Value err;
        err["result"] = "error";
        err["message"] = "Invalid request body. 'priority' must be int, 'task_id' and 'milestone_id' must be string.";
        auto resp = HttpResponse::newHttpJsonResponse(err);
        resp->setStatusCode(k400BadRequest);
        callback(resp);
        return;
    }

    const int newPriority = (*json)["priority"].asInt();
    const std::string taskId = (*json)["task_id"].asString();
    const std::string milestoneId = (*json)["milestone_id"].asString();

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

    std::string tenantId;
    try
    {
        jwt::jwt_payload jwtPayload = req->getAttributes()->get<jwt::jwt_payload>("jwt_payload");
        tenantId = jwtPayload.get_claim_value<std::string>("tenant_id");
    }
    catch (const std::exception &e)
    {
        Json::Value err;
        err["result"] = "error";
        err["message"] = std::string("Unauthorized or invalid JWT payload: ") + e.what();
        auto resp = HttpResponse::newHttpJsonResponse(err);
        resp->setStatusCode(k401Unauthorized);
        callback(resp);
        return;
    }

    DbClientPtr dbClient = drogon::app().getDbClient("teams_manager");
    auto trans = dbClient->newTransaction();

    trans->execSqlAsync(
        getTaskReprioritizeReorderSql(),
        [callback](const Result &r2) {
            Json::Value ret;
            ret["result"] = "ok";
            ret["message"] = "Tasks reprioritized successfully.";
            ret["updated_count"] = static_cast<int>(r2.affectedRows());
            auto resp = HttpResponse::newHttpJsonResponse(ret);
            callback(resp);
        },
        [callback](const DrogonDbException &e) {
            LOG_ERROR << "Task reorder error: " << e.base().what();
            Json::Value err;
            err["result"] = "error";
            err["message"] = std::string("Task reorder failed: ") + e.base().what();
            auto resp = HttpResponse::newHttpJsonResponse(err);
            resp->setStatusCode(k500InternalServerError);
            callback(resp);
        },
        milestoneId, taskId, newPriority);
    // trans->execSqlAsync(
    //     getTaskReprioritizeUpdateTargetSql(),
    //     [callback, trans, milestoneId, taskId, newPriority, tenantId](const Result &r) {
    //         if (r.affectedRows() == 0)
    //         {
    //             Json::Value err;
    //             err["result"] = "error";
    //             err["message"] = "Task not found in current tenant.";
    //             auto resp = HttpResponse::newHttpJsonResponse(err);
    //             resp->setStatusCode(k404NotFound);
    //             callback(resp);
    //             return;
    //         }

    //     },
    //     [callback](const DrogonDbException &e) {
    //         LOG_ERROR << "Task update target priority error: " << e.base().what();
    //         Json::Value err;
    //         err["result"] = "error";
    //         err["message"] = std::string("Task update target failed: ") + e.base().what();
    //         auto resp = HttpResponse::newHttpJsonResponse(err);
    //         resp->setStatusCode(k500InternalServerError);
    //         callback(resp);
    //     },
    //     newPriority, taskId, tenantId);
}