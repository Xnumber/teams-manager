#include "StatisticCtrl.h"
#include <drogon/HttpResponse.h>
#include <jsoncpp/json/json.h>
#include "utils/HttpErrorHandler.h"
#include "plugins/RequestContext/RequestContext.h"






using namespace drogon;
using namespace drogon::orm;
void StatisticCtrl::statisticGitInfoByMonth(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
{
    (void)req;
    DbClientPtr dbClient = drogon::app().getDbClient("teams_manager");
    std::string user_id = RequestContext::getUserId();

    std::string month = req->getParameter("month"); // Expecting format "YYYY-MM"
    if (month.empty())
    {
        callback(makeErrorResponse(k400BadRequest, "Missing 'month' query parameter"));
        return;
    }

    if (month.size() == 7)
    {
        month += "-01";
    }
    
    if (user_id.empty())
    {
        callback(makeErrorResponse(k401Unauthorized, "No userId in context"));
        return;
    }
    try
    {
        // Use [month_start, month_end) to avoid timezone boundary overlap.
        // auto rangeResult = dbClient->execSqlSync(
        //     R"(
        //         SELECT
        //             date_trunc('month', $1::date) AS month_start,
        //             date_trunc('month', $1::date) + interval '1 month' AS month_end
        //     )",
        //     month);

        // if (rangeResult.empty())
        // {
        //     callback(makeErrorResponse(k500InternalServerError, "Failed to compute month range"));
        //     return;
        // }

        // const std::string monthStart = rangeResult[0]["month_start"].as<std::string>();
        // const std::string monthEnd = rangeResult[0]["month_end"].as<std::string>();

        auto prRows = dbClient->execSqlSync(
            R"(
                SELECT
                    id,
                    user_id,
                    pr_number,
                    pr_user_id,
                    pr_user_login,
                    html_url,
                    title,
                    body,
                    state,
                    is_merged,
                    github_repository_id,
                    github_repository_name,
                    created_at,
                    updated_at,
                    closed_at,
                    merged_at,
                    project_id,
                    milestone_id,
                    task_id
                FROM pull_requests
                WHERE created_at >= date_trunc('month', $1::date)
                  AND created_at <  (date_trunc('month', $1::date) + interval '1 month')
                                    AND user_id = $2::uuid
                ORDER BY created_at DESC
            )",
                        month, user_id);

        auto commitRows = dbClient->execSqlSync(
            R"(
                SELECT
                    id,
                    html_url,
                    commit_sha,
                    user_id,
                    user_name,
                    github_author_id,
                    github_author_login,
                    message,
                    github_repository_id,
                    github_repository_name,
                    committed_at,
                    project_id,
                    milestone_id,
                    task_id
                FROM commits
                WHERE committed_at >= date_trunc('month', $1::date)
                  AND committed_at <  (date_trunc('month', $1::date) + interval '1 month')
                                    AND user_id = $2::uuid
                ORDER BY committed_at DESC
            )",
            month, user_id);

        Json::Value prData(Json::arrayValue);
        for (const auto &row : prRows)
        {
            Json::Value item;
            item["id"] = row["id"].isNull() ? Json::Value() : Json::Value(row["id"].as<std::string>());
            item["user_id"] = row["user_id"].isNull() ? Json::Value() : Json::Value(row["user_id"].as<std::string>());
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
            prData.append(item);
        }
        std::set<long long> repoIds;
        Json::Value commitData(Json::arrayValue);
        for (const auto &row : commitRows)
        {
            Json::Value item;
            item["id"] = row["id"].isNull() ? Json::Value() : Json::Value(row["id"].as<std::string>());
            item["html_url"] = row["html_url"].as<std::string>();
            item["commit_sha"] = row["commit_sha"].as<std::string>();
            item["user_id"] = row["user_id"].isNull() ? Json::Value() : Json::Value(row["user_id"].as<std::string>());
            item["user_name"] = row["user_name"].isNull() ? Json::Value() : Json::Value(row["user_name"].as<std::string>());
            item["github_author_id"] = Json::Value(static_cast<Json::Int64>(row["github_author_id"].as<long long>()));
            item["github_author_login"] = row["github_author_login"].as<std::string>();
            item["message"] = row["message"].as<std::string>();
            item["github_repository_id"] = Json::Value(static_cast<Json::Int64>(row["github_repository_id"].as<long long>()));
            item["github_repository_name"] = row["github_repository_name"].as<std::string>();
            item["committed_at"] = row["committed_at"].as<std::string>();
            item["project_id"] = row["project_id"].isNull() ? Json::Value() : Json::Value(row["project_id"].as<std::string>());
            item["milestone_id"] = row["milestone_id"].isNull() ? Json::Value() : Json::Value(row["milestone_id"].as<std::string>());
            item["task_id"] = row["task_id"].isNull() ? Json::Value() : Json::Value(row["task_id"].as<std::string>());
            
            if (!row["github_repository_id"].isNull())
            {
                repoIds.insert(row["github_repository_id"].as<long long>());
            }

            commitData.append(item);
        }

        Json::Value ret;
        ret["result"] = "ok";
        ret["month_start"] = month;
        // ret["month_end_exclusive"] = monthEnd;
        ret["pull_requests"] = prData;
        ret["commits"] = commitData;
        ret["pr_count"] = static_cast<int>(prData.size());
        ret["commit_count"] = static_cast<int>(commitData.size());
        ret["repository_count"] = static_cast<int>(repoIds.size());
        callback(HttpResponse::newHttpJsonResponse(ret));
    }
    catch (const std::exception &e)
    {
        callback(makeErrorResponse(k500InternalServerError, std::string("Statistic query failed: ") + e.what()));
    }
}