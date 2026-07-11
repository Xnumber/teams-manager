#include "TaskCtrl.h"
#include <jsoncpp/json/json.h>
#include <drogon/HttpResponse.h>
#include "models/Tasks.h"
#include "sql/sql.h"
#include "plugins/RequestContext/RequestContext.h"

using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::teams_manager;

void TaskCtrl::updateDependency(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback, std::string taskId)
{
    LOG_DEBUG << "Task updateDependency called: " << taskId;
    try
    {
        auto json = req->getJsonObject();
        DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");

        // 查 predecessor（dependency）：predecessor_id 為 dependency，successor_id 為輸入的 taskId
        if (!json || !json->isMember("dependency_id") || !(*json)["dependency_id"].isString())
        {
            Json::Value error;
            error["result"] = "error";
            error["message"] = "Missing or invalid dependency_id in request body";
            auto resp = HttpResponse::newHttpJsonResponse(error);
            resp->setStatusCode(k400BadRequest);
            callback(resp);
            return;
        } 
            
        std::string dependencyId = (*json)["dependency_id"].asString();
        drogon::orm::Result result = clientPtr->execSqlSync(
            updateTaskDependencySql,
            dependencyId,
            taskId
        );
        Json::Value ret;
        if (result.affectedRows() > 0)
        {
            ret["result"] = "success";
            ret["message"] = "Dependency updated successfully";
            ret["data"] = Json::arrayValue;
            // for (const auto &row : result)
            // {
            //     Json::Value rowJson;
            //     for (size_t i = 0; i < row.size(); ++i)
            //     {
            //         rowJson[result.getColumnName(i)] = row[i].as<std::string>();
            //     }
            //     ret["data"].append(rowJson);
            // }
        }
        else
        {
            ret["result"] = "error";
            ret["message"] = "No rows affected. Either the dependency already exists or the task/dependency does not exist.";
        }
        

        drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(ret);
        callback(resp);
    }
    catch (const DrogonDbException &e)
    {
        LOG_ERROR << "Database error: " << e.base().what();
        Json::Value error;
        error["result"] = "error";
        error["message"] = e.base().what();
        auto resp = HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(k500InternalServerError);
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
