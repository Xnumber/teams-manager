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
 */
void TaskCtrl::updateScheduledStartDate(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback)
{
    LOG_DEBUG << "Task updateScheduledStartDate called";
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

        std::string taskId = json->get("task_id", "").asString();
        std::string scheduledStartDate = json->get("scheduled_start_date", "").asString();

        DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");

        if (taskId.empty())
        {
            Json::Value error;
            error["result"] = "error";
            error["message"] = "task_id is required";
            auto resp = HttpResponse::newHttpJsonResponse(error);
            resp->setStatusCode(k400BadRequest);
            callback(resp);
            return;
        }

        if (scheduledStartDate.empty())
        {
            Json::Value error;
            error["result"] = "error";
            error["message"] = "scheduled_start_date is required";
            auto resp = HttpResponse::newHttpJsonResponse(error);
            resp->setStatusCode(k400BadRequest);
            callback(resp);
            return;
        }

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
            scheduledStartDate,
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
