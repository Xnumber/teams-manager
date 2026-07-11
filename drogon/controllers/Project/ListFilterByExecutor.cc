#include "ProjectCtrl.h"
#include <jsoncpp/json/json.h>
#include <drogon/HttpResponse.h>
#include "jwt/jwt.hpp"
#include "sql/list.h"
#include "sql/sql.h"

using namespace drogon;
using namespace drogon::orm;
// using namespace drogon_model::teams_manager;

void Project::listFilterByExecutor(const HttpRequestPtr &req,
                                   std::function<void(const HttpResponsePtr &)> &&callback)
{
    LOG_DEBUG << "Project listFilterByExecutor called";

    // get executor_id from query parameter
    std::string executorId = req->getParameter("executor_id");
    if (executorId.empty())
    {
        Json::Value error;
        error["result"] = "error";
        error["message"] = "Missing executor_id parameter";
        auto resp = HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(k400BadRequest);
        callback(resp);
        return;
    }

    // SQL: return projects that have at least one task assigned to executor
    const std::string sql = R"SQL(
        SELECT
            pr.name,
            pr.priority,
            pr.description,
            COUNT(DISTINCT ms.id) FILTER (WHERE COALESCE(ms.completed, false) = false)::INT AS milestones_count_in_queue,
            COUNT(DISTINCT tk_count.id) FILTER (WHERE tk_count.status_name = '排隊中' OR LOWER(tk_count.status_name) = 'queued')::INT AS tasks_count_in_queue,
            pr.id,
            pr.concurrency_stamp,
            pr.created_at,
            pr.tenant_id
        FROM projects pr
        LEFT JOIN milestones ms ON ms.project_id = pr.id
        LEFT JOIN tasks tk_count ON tk_count.project_id = pr.id AND tk_count.completed = false
        WHERE EXISTS (
            SELECT 1
            FROM tasks t
            JOIN executors_tasks et ON et.task_id = t.id
            WHERE t.project_id = pr.id
            AND et.user_id = $1
        )
        GROUP BY
            pr.name,
            pr.priority,
            pr.description,
            pr.id,
            pr.concurrency_stamp,
            pr.created_at,
            pr.tenant_id
        ORDER BY pr.created_at DESC
    )SQL";

    try
    {
        DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
        clientPtr->execSqlAsync(
            sql,
            [callback](const Result &r) {
                Json::Value projects(Json::arrayValue);
                for (const auto &row : r)
                {
                    Json::Value item;
                    item["name"] = row["name"].as<std::string>();
                    item["priority"] = row["priority"].as<int>();
                    item["description"] = row["description"].isNull() ? "" : row["description"].as<std::string>();
                    item["milestones_count_in_queue"] = row["milestones_count_in_queue"].as<int>();
                    item["tasks_count_in_queue"] = row["tasks_count_in_queue"].as<int>();
                    item["id"] = row["id"].as<std::string>();
                    item["concurrency_stamp"] = row["concurrency_stamp"].as<std::string>();
                    item["created_at"] = row["created_at"].as<std::string>();
                    item["tenant_id"] = row["tenant_id"].as<std::string>();
                    projects.append(item);
                }
                Json::Value ret;
                ret["result"] = "ok";
                ret["data"] = projects;
                ret["count"] = static_cast<int>(projects.size());
                auto resp = HttpResponse::newHttpJsonResponse(ret);
                callback(resp);
            },
            [callback](const DrogonDbException &e) {
                LOG_ERROR << "Database error: " << e.base().what();
                Json::Value error;
                error["result"] = "error";
                error["message"] = e.base().what();
                auto resp = HttpResponse::newHttpJsonResponse(error);
                resp->setStatusCode(k500InternalServerError);
                callback(resp);
            },
            executorId
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
}
