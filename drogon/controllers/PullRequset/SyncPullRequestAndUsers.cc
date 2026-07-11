#include "PullRequestCtrl.h"
#include "plugins/RequestContext/RequestContext.h"
#include "utils/HttpErrorHandler.h"
using namespace drogon;
using namespace drogon::orm;

void PullRequestCtrl::syncPullRequestsAndUsers(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
{
//     // 目前的實作與 syncPullRequests 基本相同，未來可以根據需求進行擴展，例如在同步 pull request 的同時也同步相關的使用者資料
//     // syncPullRequests(req, std::move(callback));
    DbClientPtr dbClient = drogon::app().getDbClient("teams_manager");
//     // 只同步 git_user_links 存在的 git_user_id 對應 user_id 到 pull_requests
    dbClient->execSqlAsync(
        R"(
            UPDATE pull_requests pr
            SET user_id = gul.user_id
            FROM git_user_links gul
            WHERE pr.pr_user_id = gul.git_user_id
        )",
        [callback](const Result &rows) {
            Json::Value ret;
            ret["result"] = "ok";
            ret["updated_count"] = rows.affectedRows();
            callback(HttpResponse::newHttpJsonResponse(ret));
        },
        [callback](const DrogonDbException &e) {
            callback(makeErrorResponse(k500InternalServerError, std::string("Database error: ") + e.base().what()));
        });
}