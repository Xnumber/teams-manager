#include "TaskCtrl.h"
#include <jsoncpp/json/json.h>
#include <drogon/HttpResponse.h>
#include "sql/sql.h"
#include "models/Tasks.h"
#include "utils/UuidValidator.h"
using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::teams_manager;



/**
 * 删除任務的依賴關係
 * @param req HttpRequestPtr
 * @param callback 回調函數
 * @param taskId 工作任務ID
 */
void TaskCtrl::removeDependency(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback
) {
    LOG_DEBUG << "Task removeDependency called";
    try
    {
        const std::shared_ptr<Json::Value> json = req->getJsonObject();
        if (!json)
        {
            Json::Value error;
            error["result"] = "error";
            error["message"] = "Invalid JSON body";
            auto resp = HttpResponse::newHttpJsonResponse(error);
            resp->setStatusCode(k400BadRequest);
            callback(resp);
            return;
        }

        std::string predecessorId = (*json)["predecessor_id"].asString();
        std::string successorId = (*json)["successor_id"].asString();
        DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
        
        if (successorId.empty() || predecessorId.empty())
        {
            Json::Value error;
            error["result"] = "error";
            error["message"] = "predecessor_id and successor_id cannot be empty";
            drogon::HttpResponsePtr resp = drogon::HttpResponse::newHttpJsonResponse(error);
            resp->setStatusCode(k400BadRequest);
            callback(resp);
            return;
        }

        if (!isValidUuid(predecessorId) || !isValidUuid(successorId))
        {
            Json::Value error;
            error["result"] = "error";
            error["message"] = "predecessor_id and successor_id must be valid UUID";
            auto resp = HttpResponse::newHttpJsonResponse(error);
            resp->setStatusCode(k400BadRequest);
            callback(resp);
            return;
        }
        
        
        drogon::orm::Result result = clientPtr->execSqlSync(
            deleteTaskDependencySql,
            predecessorId,
            successorId
        );

        Json::Value ret;
        if (result.affectedRows() > 0)
        {
            ret["result"] = "success";
            ret["message"] = "Dependency removed successfully";
        }
        else
        {
            ret["result"] = "error";
            ret["message"] = "No dependency removed";
        }

        auto resp = HttpResponse::newHttpJsonResponse(ret);
        callback(resp);
    }
    catch (const DrogonDbException &e)
    {
        LOG_ERROR << "Database error: " << e.base().what();
        Json::Value error;
        error["result"] = "error";
        error["message"] = e.base().what();
        drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(k500InternalServerError);
        callback(resp);
    }
    catch (const std::exception &e)
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