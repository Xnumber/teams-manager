#include "PullRequestCtrl.h"
#include <drogon/HttpClient.h>
#include <drogon/HttpResponse.h>
#include <jsoncpp/json/json.h>
#include <algorithm>
#include <functional>
#include <memory>
#include <sstream>
#include <string>
#include "models/PullRequests.h"
using namespace drogon;
using namespace drogon::orm;

void PullRequestCtrl::list(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
{
    const std::string repoIdStr = req->getParameter("repo_id");
    if (repoIdStr.empty()) {
        Json::Value err;
        err["result"] = "error";
        err["message"] = "Missing required query param: repo_id";
        auto resp = HttpResponse::newHttpJsonResponse(err);
        resp->setStatusCode(k400BadRequest);
        callback(resp);
        return;
    }

    int64_t repoId = 0;
    try {
        repoId = std::stoll(repoIdStr);
    } catch (...) {
        Json::Value err;
        err["result"] = "error";
        err["message"] = "Invalid repo_id";
        auto resp = HttpResponse::newHttpJsonResponse(err);
        resp->setStatusCode(k400BadRequest);
        callback(resp);
        return;
    }
    DbClientPtr dbClient = drogon::app().getDbClient("teams_manager");
    try {
        auto result = dbClient->execSqlSync(
            R"(
                SELECT
                    pr.id,
                    pr.user_id,
                    u.username AS user_name,
                    pr.pr_number,
                    pr.pr_user_id,
                    pr.pr_user_login,
                    pr.html_url,
                    pr.title,
                    pr.body,
                    pr.state,
                    pr.is_merged,
                    pr.github_repository_id,
                    pr.github_repository_name,
                    pr.created_at,
                    pr.updated_at,
                    pr.closed_at,
                    pr.merged_at,
                    pr.project_id,
                    pr.milestone_id,
                    pr.task_id
                FROM pull_requests pr
                LEFT JOIN users u ON u.id = pr.user_id
                WHERE pr.github_repository_id = $1
                ORDER BY pr.updated_at DESC NULLS LAST, pr.created_at DESC
            )",
            repoId);

        Json::Value data(Json::arrayValue);
        for (const auto &row : result) {
            Json::Value item;
            item["id"] = row["id"].isNull() ? Json::Value() : Json::Value(row["id"].as<std::string>());
            item["user_id"] = row["user_id"].isNull() ? Json::Value() : Json::Value(row["user_id"].as<std::string>());
            item["user_name"] = row["user_name"].isNull() ? Json::Value() : Json::Value(row["user_name"].as<std::string>());
            item["pr_number"] = row["pr_number"].as<int>();
            item["pr_user_id"] = Json::Value(static_cast<Json::Int64>(row["pr_user_id"].as<long long>()));
            item["pr_user_login"] = row["pr_user_login"].as<std::string>();
            item["html_url"] = row["html_url"].as<std::string>();
            item["title"] = row["title"].as<std::string>();
            item["body"] = row["body"].as<std::string>();
            item["state"] = row["state"].as<std::string>();
            item["is_merged"] = row["is_merged"].as<bool>();
            item["github_repository_id"] = row["github_repository_id"].isNull()
                ? Json::Value()
                : Json::Value(static_cast<Json::Int64>(row["github_repository_id"].as<long long>()));
            item["github_repository_name"] = row["github_repository_name"].as<std::string>();
            item["created_at"] = row["created_at"].as<std::string>();
            item["updated_at"] = row["updated_at"].isNull() ? Json::Value() : Json::Value(row["updated_at"].as<std::string>());
            item["closed_at"] = row["closed_at"].isNull() ? Json::Value() : Json::Value(row["closed_at"].as<std::string>());
            item["merged_at"] = row["merged_at"].isNull() ? Json::Value() : Json::Value(row["merged_at"].as<std::string>());
            item["project_id"] = row["project_id"].isNull() ? Json::Value() : Json::Value(row["project_id"].as<std::string>());
            item["milestone_id"] = row["milestone_id"].isNull() ? Json::Value() : Json::Value(row["milestone_id"].as<std::string>());
            item["task_id"] = row["task_id"].isNull() ? Json::Value() : Json::Value(row["task_id"].as<std::string>());

            data.append(item);
        }

        Json::Value ret;
        ret["result"] = "ok";
        ret["data"] = data;
        ret["count"] = static_cast<int>(data.size());
        callback(HttpResponse::newHttpJsonResponse(ret));
    } catch (const std::exception &e) {
        Json::Value err;
        err["result"] = "error";
        err["message"] = std::string("Query failed: ") + e.what();
        auto resp = HttpResponse::newHttpJsonResponse(err);
        resp->setStatusCode(k500InternalServerError);
        callback(resp);
    }
}