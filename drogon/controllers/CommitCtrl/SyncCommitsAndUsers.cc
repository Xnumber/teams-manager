#include "CommitCtrl.h"
#include "utils/HttpErrorHandler.h"
using namespace drogon;
using namespace drogon::orm;

void CommitCtrl::syncCommitsAndUsers(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
{
    DbClientPtr dbClient = drogon::app().getDbClient("teams_manager");

    dbClient->execSqlAsync(
        R"(
            UPDATE commits c
            SET user_id = gul.user_id,
                user_name = COALESCE(u.username, c.user_name)
            FROM git_user_links gul
            LEFT JOIN users u ON u.id = gul.user_id
            WHERE c.github_author_id = gul.git_user_id
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
