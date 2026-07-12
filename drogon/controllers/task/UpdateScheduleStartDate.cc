#include "TaskCtrl.h"
#include <jsoncpp/json/json.h>
#include <drogon/HttpResponse.h>
#include "sql/sql.h"
#include "models/Tasks.h"
using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::teams_manager;


/**
 * 更新任务的計畫開始日期
 * @param req HttpRequestPtr
 * @param callback 回调函数
 * @param taskId 工作任務ID
 */
void TaskCtrl::updateScheduledStartDate(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback, std::string taskId)
{
    LOG_DEBUG << "Task updateScheduledStartDate called: " << taskId;
    try
    {
        auto json = req->getJsonObject();
        DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
        clientPtr->execSqlAsync(
            updateTaskStartDateSql,
            [callback](const Result &r) {
                Json::Value ret;
                ret["result"] = "success";
                drogon::HttpResponsePtr resp = drogon::HttpResponse::newHttpJsonResponse(ret);
                callback(resp);
            },
            [callback](const DrogonDbException &e) {
                LOG_ERROR << "Database error: " << e.base().what();
                Json::Value error;
                error["result"] = "error";
                error["message"] = e.base().what();
                auto resp = drogon::HttpResponse::newHttpJsonResponse(error);
                resp->setStatusCode(k500InternalServerError);
                callback(resp);
            },
            json->get("scheduled_start_date", "").asString(),
            taskId
        );
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
