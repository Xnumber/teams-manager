#include "OrganizationCtrl.h"
#include "get-overview.h"
#include <jsoncpp/json/json.h>
#include <drogon/HttpResponse.h>

using namespace drogon;
using namespace drogon::orm;


void OrganizationCtrl::overview(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback) const
{
    (void)req;
    LOG_DEBUG << "Organization overview called";

    DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
    clientPtr->execSqlAsync(
        getOrganizationOverviewSql,
        [callback](const Result &r)
        {
            if (r.empty())
            {
                Json::Value error;
                error["result"] = "error";
                error["message"] = "organization overview not found";
                HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(error);
                resp->setStatusCode(k404NotFound);
                callback(resp);
                return;
            }

            const auto &row = r[0];
            Json::Value data;
            data["teams_count"] = row["teams_count"].isNull() ? 0 : row["teams_count"].as<int>();
            data["projects_count"] = row["projects_count"].isNull() ? 0 : row["projects_count"].as<int>();
            data["users_count"] = row["users_count"].isNull() ? 0 : row["users_count"].as<int>();
            data["members_count"] = row["members_count"].isNull() ? 0 : row["members_count"].as<int>();
            data["tasks_count"] = row["tasks_count"].isNull() ? 0 : row["tasks_count"].as<int>();
            data["tasks_estimated_workdays"] =
                row["tasks_estimated_workdays"].isNull()
                    ? Json::Value()
                    : Json::Value(row["tasks_estimated_workdays"].as<double>());
            data["milestones_count"] = row["milestones_count"].isNull() ? 0 : row["milestones_count"].as<int>();

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
        });
}