

#include "ProjectCtrl.h"
#include "sql/get-overview.h"
#include <jsoncpp/json/json.h>
#include <drogon/HttpResponse.h>
#include <regex>

using namespace drogon;
using namespace drogon::orm;

void Project::getOverview(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback,
    std::string projectId)
{
    LOG_DEBUG << "Project getOverview called";

    if (projectId.empty())
    {
        Json::Value error;
        error["result"] = "error";
        error["message"] = "project_id is empty";
        HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(k400BadRequest);
        callback(resp);
        return;
    }

    static const std::regex uuidPattern(
        "^[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[1-5][0-9a-fA-F]{3}-[89abAB][0-9a-fA-F]{3}-[0-9a-fA-F]{12}$");
    if (!std::regex_match(projectId, uuidPattern))
    {
        Json::Value error;
        error["result"] = "error";
        error["message"] = "project_id is not a valid UUID";
        HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(k400BadRequest);
        callback(resp);
        return;
    }

    DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
    clientPtr->execSqlAsync(
        getProjectOverviewSql,
        [callback, projectId](const Result &r)
        {
            if (r.empty())
            {
                Json::Value error;
                error["result"] = "error";
                error["message"] = "project not found";
                error["project_id"] = projectId;
                HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(error);
                resp->setStatusCode(k404NotFound);
                callback(resp);
                return;
            }

            const auto &row = r[0];
            Json::Value data;
            data["project_id"] = row["project_id"].as<std::string>();
            data["remaining_milestones_count"] = row["remaining_milestones_count"].as<int>();
            data["participating_teams_count"] = row["participating_teams_count"].as<int>();
            data["executors_count"] = row["executors_count"].as<int>();
            data["latest_complete_date"] = row["latest_complete_date"].isNull()
                ? Json::Value()
                : Json::Value(row["latest_complete_date"].as<std::string>());
            data["earliest_incomplete_demo_milestone_name"] = row["earliest_incomplete_demo_milestone_name"].isNull()
                ? Json::Value()
                : Json::Value(row["earliest_incomplete_demo_milestone_name"].as<std::string>());
            data["earliest_incomplete_demo_milestone_complete_date"] = row["earliest_incomplete_demo_milestone_complete_date"].isNull()
                ? Json::Value()
                : Json::Value(row["earliest_incomplete_demo_milestone_complete_date"].as<std::string>());

            Json::Value ret;
            ret["result"] = "ok";
            ret["data"] = data;

            HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(ret);
            callback(resp);
        },
        [callback](const DrogonDbException &e)
        {
            LOG_ERROR << "Database error: " << e.base().what();
            Json::Value error;
            error["result"] = "error";
            error["message"] = e.base().what();
            HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(error);
            resp->setStatusCode(k500InternalServerError);
            callback(resp);
        },
        projectId);
}
