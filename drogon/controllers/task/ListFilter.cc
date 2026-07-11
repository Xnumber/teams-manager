#include "TaskCtrl.h"
#include <drogon/HttpResponse.h>
#include <jsoncpp/json/json.h>
#include "plugins/RequestContext/RequestContext.h"
#include "sql/filter-list.h"
using namespace drogon;
using namespace drogon::orm;


void TaskCtrl::listFilter(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback)
{
    std::string team_id = req->getParameter("team_id");
    std::string milestone_id = req->getParameter("milestone_id");
    std::string project_id = req->getParameter("project_id");
    std::string completedInput = req->getParameter("completed");
    std::string executor_id = req->getParameter("executor_id");
    std::string name = req->getParameter("name");

    std::string completed = completedInput;

    LOG_DEBUG << "Task listFilter called";
    DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
    try
    {
        if(!completed.empty())
        {
            if (completed == "true" || completed == "1")
            {
                completed = "true";
            }
            else if (completed == "false" || completed == "0")
            {
                completed = "false";
            }
            else
            {
                Json::Value error;
                error["result"] = "error";
                error["message"] = "Invalid completed value. Use true/false or 1/0.";
                drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(error);
                resp->setStatusCode(k400BadRequest);
                callback(resp);
                return;
            }
        }

        drogon::orm::Result result = clientPtr->execSqlSync(
            filterListSql,
            team_id,
            milestone_id,
            project_id,
            completed,
            executor_id,
            name
        );

        Json::Value data(Json::arrayValue);
        int count = 0;

        if (!result.empty())
        {
            Json::Reader reader;
            reader.parse(result[0]["data"].as<std::string>(), data);
            count = result[0]["count"].as<int>();
        }

        Json::Value ret;
        ret["result"] = "ok";
        ret["data"] = data;
        ret["count"] = count;
        drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(ret);
        callback(resp);
    } catch (const std::exception &e)
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