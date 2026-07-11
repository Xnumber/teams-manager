#include "StatisticCtrl.h"
#include <drogon/HttpResponse.h>
#include <jsoncpp/json/json.h>
#include "utils/HttpErrorHandler.h"
#include "plugins/RequestContext/RequestContext.h"
#include "WorkLoadWeight.cc"
using namespace drogon;
using namespace drogon::orm;

void StatisticCtrl::statisticByMonth(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{
    (void)req;

    std::string teamId = req->getParameter("team_id");
    std::string month = req->getParameter("month");

    if (month.empty())
    {
        callback(makeErrorResponse(k400BadRequest, "Missing required query param: month"));
        return;
    }

    if (month.size() == 7)
    {
        month += "-01";
    }

    DbClientPtr dbClient = drogon::app().getDbClient("teams_manager");
    std::string user_id = RequestContext::getUserId();

    try
    {
        auto rangeResult = dbClient->execSqlSync(
            R"(
                SELECT
                    date_trunc('month', $1::timestamp) AS month_start,
                    date_trunc('month', $1::timestamp) + interval '1 month' AS month_end
            )",
            month);

        if (rangeResult.empty())
        {
            callback(makeErrorResponse(k500InternalServerError, "Failed to compute month range"));
            return;
        }

        std::string monthStart = rangeResult[0]["month_start"].as<std::string>();
        std::string monthEnd = rangeResult[0]["month_end"].as<std::string>();

        Json::Value tasksStatistics(Json::arrayValue);
        drogon::orm::Result tasksStatisticsResult = dbClient->execSqlSync(

            R"(
                SELECT 
                    u.username,
                    COUNT(DISTINCT t.id) AS tasks_count,
                    COUNT(DISTINCT t.project_id) FILTER (WHERE t.project_id IS NOT NULL) AS participated_projects_count,
                    COUNT(DISTINCT c.github_repository_id) AS participated_repository_count,
                    COUNT(DISTINCT pr.id) AS pull_request_count,
                    COUNT(DISTINCT c.id) AS commit_count
                FROM users u
                LEFT JOIN (
                    -- 通過 executors_tasks 表關聯
                    SELECT et.user_id, t.id, t.project_id
                    FROM executors_tasks et
                    INNER JOIN tasks t ON et.task_id = t.id
                    WHERE t.completed = true
                      AND t.team_id = $1
                      AND DATE_TRUNC('month', t.created_at) = DATE_TRUNC('month', $2::timestamp)
                    
                    UNION ALL
                    
                    -- 通過 mentors_tasks 表關聯
                    SELECT mt.user_id, t.id, t.project_id
                    FROM mentors_tasks mt
                    INNER JOIN tasks t ON mt.task_id = t.id
                    WHERE t.completed = true
                      AND t.team_id = $1
                      AND DATE_TRUNC('month', t.created_at) = DATE_TRUNC('month', $2::timestamp)
                ) t ON u.id = t.user_id
                LEFT JOIN pull_requests pr
                    ON pr.user_id = u.id
                    AND pr.created_at >= DATE_TRUNC('month', $2::timestamp)
                    AND pr.created_at < (DATE_TRUNC('month', $2::timestamp) + INTERVAL '1 month')
                LEFT JOIN commits c
                    ON c.user_id = u.id
                    AND c.committed_at >= DATE_TRUNC('month', $2::timestamp)
                    AND c.committed_at < (DATE_TRUNC('month', $2::timestamp) + INTERVAL '1 month')
                WHERE u.team_id = $1
                GROUP BY u.id, u.username
                ORDER BY u.username
                )",
            teamId, month);



        double team_workload = 0.0;
        double team_contribution = 0.0;
        for (const auto &row : tasksStatisticsResult)
        {
            if (row["username"].as<std::string>() == "clement")
            {
                continue;
            }
            Json::Value item;

            const long long tasksCount = row["tasks_count"].as<long long>();
            const long long pullRequestCount = row["pull_request_count"].as<long long>();
            const long long commitCount = row["commit_count"].as<long long>();
            const long long participatedProjectsCount = row["participated_projects_count"].as<long long>();
            const long long participatedRepositoryCount = row["participated_repository_count"].as<long long>();

            const double taskWeight = workload_weight::projectWeightByCount(
                static_cast<int>(participatedProjectsCount));
            const double taskWorkload = static_cast<double>(tasksCount) * 10.0 * taskWeight;
            
            const double githubWeight = workload_weight::githubWeightByCount(
                static_cast<int>(participatedRepositoryCount));
            const double githubWorkload = (static_cast<double>(pullRequestCount) + static_cast<double>(commitCount)) * githubWeight;

            const double taskContribution = tasksCount * 20; // 這裡可以根據實際需求調整工作貢獻度的計算方式
            
            const double githubContribution = pullRequestCount * 10 + commitCount; // 這裡可以根據實際需求調整 GitHub 貢獻度的計算方式


            
            
            const double totalWorkload = taskWorkload + githubWorkload;
            const double totalContribution = taskContribution + githubContribution; // 這裡可以根據實際需求調整貢獻度的計算方式

            item["username"] = row["username"].as<std::string>();
            item["tasks_count"] = Json::Value(static_cast<Json::Int64>(row["tasks_count"].as<long long>()));
            item["participated_projects_count"] = Json::Value(static_cast<Json::Int64>(row["participated_projects_count"].as<long long>()));
            item["participated_repository_count"] = Json::Value(static_cast<Json::Int64>(row["participated_repository_count"].as<long long>()));
            item["pull_request_count"] = Json::Value(static_cast<Json::Int64>(row["pull_request_count"].as<long long>()));
            item["commit_count"] = Json::Value(static_cast<Json::Int64>(row["commit_count"].as<long long>()));
            
            item["task_workload"] = Json::Value(taskWorkload);
            item["task_contribution"] = Json::Value(taskContribution);
            item["github_workload"] = Json::Value(githubWorkload);
            item["github_contribution"] = Json::Value(githubContribution);
            item["total_workload"] = Json::Value(totalWorkload);
            item["total_contribution"] = Json::Value(totalContribution);


            


            


            team_workload += totalWorkload;
            team_contribution += totalContribution;
            tasksStatistics.append(item);
        }





        


        Json::Value ret;
        ret["result"] = "ok";
        ret["data"] = tasksStatistics;
        ret["month_start"] = monthStart;
        ret["month_end"] = monthEnd;
        ret["team_workload"] = team_workload;
        ret["team_contribution"] = team_contribution;

        callback(HttpResponse::newHttpJsonResponse(ret));
    }
    catch (const std::exception &e)
    {
        callback(makeErrorResponse(k500InternalServerError, std::string("Statistic query failed: ") + e.what()));
    }
}