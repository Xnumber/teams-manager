#include "ProjectCtrl.h"
#include <jsoncpp/json/json.h>
#include <drogon/HttpResponse.h>
#include "models/Projects.h"
#include "jwt/jwt.hpp"
#include "sql/reprioritize-sql.h"

using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::teams_manager;



/**
 * 輸入有 priority, id。
 * 取得 priority後，根據 priority 重新排序所有 project。
 */

void Project::reprioritize(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback
)
{
    const std::shared_ptr<Json::Value> json = req->getJsonObject();
    std::string tenantId = (*json)["tenant_id"].asString();
    if (!json || !(*json)["priority"].isInt() || !(*json)["project_id"].isString()) {
        Json::Value err;
        err["result"] = "error";
        err["message"] = "Invalid request body. 'priority' must be int and 'project_id' must be string.";
        auto resp = HttpResponse::newHttpJsonResponse(err);
        resp->setStatusCode(k400BadRequest);
        callback(resp);
        return;
    }

    const int newPriority = (*json)["priority"].asInt();
    const std::string projectId = (*json)["project_id"].asString();

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
        getReprioritizeUpdateTargetSql(),
        [callback, trans, projectId, newPriority, tenantId](const Result &r) {
            if (r.affectedRows() == 0)
            {
                Json::Value err;
                err["result"] = "error";
                err["message"] = "Project not found in current tenant.";
                auto resp = HttpResponse::newHttpJsonResponse(err);
                resp->setStatusCode(k404NotFound);
                callback(resp);
                return;
            }

            trans->execSqlAsync(
                getReprioritizeReorderSql(),
                [callback](const Result &r2) {
                    Json::Value ret;
                    ret["result"] = "ok";
                    ret["message"] = "Projects reprioritized successfully.";
                    ret["updated_count"] = static_cast<int>(r2.affectedRows());
                    auto resp = HttpResponse::newHttpJsonResponse(ret);
                    callback(resp);
                },
                [callback](const DrogonDbException &e) {
                    LOG_ERROR << "Reorder error: " << e.base().what();
                    Json::Value err;
                    err["result"] = "error";
                    err["message"] = std::string("Reorder failed: ") + e.base().what();
                    auto resp = HttpResponse::newHttpJsonResponse(err);
                    resp->setStatusCode(k500InternalServerError);
                    callback(resp);
                },
                projectId, newPriority);
        },
        [callback](const DrogonDbException &e) {
            LOG_ERROR << "Update target priority error: " << e.base().what();
            Json::Value err;
            err["result"] = "error";
            err["message"] = std::string("Update target failed: ") + e.base().what();
            auto resp = HttpResponse::newHttpJsonResponse(err);
            resp->setStatusCode(k500InternalServerError);
            callback(resp);
        },
        newPriority, projectId);
}