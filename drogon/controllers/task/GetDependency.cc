#include "TaskCtrl.h"
#include "TaskCtrl.h"
#include <jsoncpp/json/json.h>
#include <drogon/HttpResponse.h>
#include "models/Tasks.h"
#include "plugins/RequestContext/RequestContext.h"

using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::teams_manager;

void TaskCtrl::getDependency(const HttpRequestPtr &req,
                            std::function<void(const HttpResponsePtr &)> &&callback,
                            std::string taskId)
{
    LOG_DEBUG << "Task getDependency called: " << taskId;
    try
    {
        DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");

        // 查 predecessor（dependency）：predecessor_id 為 dependency，successor_id 為輸入的 taskId
        const char *sql = R"(
            SELECT t.id::text AS id, t.name
            FROM work_item_dependencies w
            JOIN tasks t ON t.id = w.predecessor_id
            WHERE w.successor_id = $1
            LIMIT 1
        )";

        orm::Result result = clientPtr->execSqlSync(sql, taskId);

        Json::Value ret;
        ret["result"] = "ok";

        if (result.empty())
        {
            ret["data"] = Json::nullValue;
        }
        else
        {
            const Row &row = result.front();
            Json::Value data;
            data["id"] = row["id"].as<std::string>();
            data["name"] = row["name"].as<std::string>();
            ret["data"] = data;
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
