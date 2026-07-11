#include "TaskCtrl.h"
#include <jsoncpp/json/json.h>
#include <drogon/HttpResponse.h>
#include "plugins/RequestContext/RequestContext.h"
#include "sql/set-complete-sql.h"
#include "models/Tasks.h"

using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::teams_manager;
/**
 * 將指定的 task 標記為已完成
 * 1. 更新 task 的 表的 completed 欄位為 true
 * 2. 更新 task 的 completion_date 為當前日期
 */
void TaskCtrl::setTaskcompleted(const HttpRequestPtr &req,
                                std::function<void(const HttpResponsePtr &)> &&callback, std::string taskId)
{
    LOG_DEBUG << "Task setTaskcompleted called";
    DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
    orm::Mapper<Tasks> mapper(clientPtr);
    try
    {
        clientPtr->execSqlSync(setCompleteSql, taskId);
        Json::Value ret;
        ret["result"] = "ok";
        // ret["task"] = task.toJson();
        auto resp = HttpResponse::newHttpJsonResponse(ret);
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