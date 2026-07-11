#include "CommitCtrl.h"
#include <drogon/HttpClient.h>
#include <drogon/HttpResponse.h>
#include <jsoncpp/json/json.h>
#include <algorithm>
#include <functional>
#include <memory>
#include <sstream>
#include <string>
#include "models/Repositories.h"
#include "models/Users.h"
#include "models/GitUserLinks.h"
#include "plugins/RequestContext/RequestContext.h"
#include "utils/HttpErrorHandler.h"
using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::teams_manager;




namespace
{
struct SyncState
{
    DbClientPtr dbClient;
    HttpClientPtr ghClient;
    std::string userId;
    std::string userName;
    std::string owner;
    std::string repo;
    int64_t repoId{0};
    std::string token;
    std::string since;
    std::string until;
    std::string sha;
    int perPage{100};
    int maxPages{10};
    Json::Value commits{Json::arrayValue};
};

std::string truncateTo255(const std::string &value)
{
    if (value.size() <= 255)
    {
        return value;
    }
    return value.substr(0, 255);
}
} // namespace
void CommitCtrl::syncCommits(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
{
    auto json = req->getJsonObject();
    if (!json)
    {
        callback(makeErrorResponse(k400BadRequest, "Invalid JSON body"));
        return;
    }

    if (!(*json).isMember("repo_id"))
    {
        callback(makeErrorResponse(k400BadRequest, "Missing required field: repo_id"));
        return;
    }

    if (!(*json).isMember("github_token") || (*json)["github_token"].asString().empty())
    {
        callback(makeErrorResponse(k400BadRequest, "Missing required field: github_token"));
        return;
    }

    const int64_t repoId = (*json)["repo_id"].asInt64();
    const std::string githubToken = (*json)["github_token"].asString();
    const std::string since = (*json).isMember("since") ? (*json)["since"].asString() : "";
    const std::string until = (*json).isMember("until") ? (*json)["until"].asString() : "";
    const std::string sha = (*json).isMember("sha") ? (*json)["sha"].asString() : "develop";
    // const std::string sha = "develop";
    const int perPage = (*json).isMember("per_page") ? std::clamp((*json)["per_page"].asInt(), 1, 100) : 100;
    const int maxPages = (*json).isMember("max_pages") ? std::clamp((*json)["max_pages"].asInt(), 1, 50) : 10;

    DbClientPtr dbClient = drogon::app().getDbClient("teams_manager");
    Mapper<Repositories> repoMapper(dbClient);
    Mapper<Users> userMapper(dbClient);

    Repositories repoObj;
    try
    {
        repoObj = repoMapper.findByPrimaryKey(repoId);
    }
    catch (const std::exception &e)
    {
        callback(makeErrorResponse(k404NotFound, std::string("Repository not found: ") + e.what()));
        return;
    }

    const std::string userId = RequestContext::getUserId();
    if (userId.empty())
    {
        callback(makeErrorResponse(k401Unauthorized, "No userId in context"));
        return;
    }

    std::string userName;
    try
    {
        auto userObj = userMapper.findByPrimaryKey(userId);
        userName = userObj.getValueOfUsername();
    }
    catch (const std::exception &)
    {
        if ((*json).isMember("user_name"))
        {
            userName = (*json)["user_name"].asString();
        }
        if (userName.empty())
        {
            callback(makeErrorResponse(k404NotFound, "User not found for current request context"));
            return;
        }
    }

    auto state = std::make_shared<SyncState>();
    state->dbClient = dbClient;
    state->ghClient = HttpClient::newHttpClient("https://api.github.com");

    orm::Mapper<GitUserLinks> gitUserLinkMapper(dbClient);
    // drogon_model::teams_manager::GitUserLinks gitUserLink = gitUserLinkMapper.findByPrimaryKey(userId);
    
    // state->userId = nullptr;
    // state->userName = nullptr;
    state->owner = repoObj.getValueOfOwner();
    state->repo = repoObj.getValueOfRepoName();
    state->repoId = repoId;
    state->token = githubToken;
    state->since = since;
    state->until = until;
    state->sha = sha;
    state->perPage = perPage;
    state->maxPages = maxPages;

    std::shared_ptr<std::function<void(int)>> fetchAndSync = std::make_shared<std::function<void(int)>>();
    *fetchAndSync = [state, callback, fetchAndSync](int page) {
        auto ghReq = HttpRequest::newHttpRequest();
        ghReq->setMethod(Get);

        std::string path = "/repos/" + state->owner + "/" + state->repo + "/commits?";
        path += "per_page=" + std::to_string(state->perPage);
        path += "&page=" + std::to_string(page);
        if (!state->since.empty())
        {
            path += "&since=" + state->since;
        }
        if (!state->until.empty())
        {
            path += "&until=" + state->until;
        }
        if (!state->sha.empty())
        {
            path += "&sha=" + state->sha;
        }

        ghReq->setPath(path);
        ghReq->addHeader("Accept", "application/vnd.github+json");
        ghReq->addHeader("X-GitHub-Api-Version", "2022-11-28");
        ghReq->addHeader("Authorization", "Bearer " + state->token);

        state->ghClient->sendRequest(
            ghReq,
            [state, callback, fetchAndSync, page](ReqResult result, const HttpResponsePtr &resp) {
                if (result != ReqResult::Ok || !resp)
                {
                    callback(makeErrorResponse(k502BadGateway, "Failed to request GitHub API"));
                    return;
                }

                if (resp->statusCode() != k200OK)
                {
                    callback(makeErrorResponse(
                        k502BadGateway,
                        "GitHub API error: HTTP " + std::to_string(static_cast<int>(resp->statusCode())) +
                            " body=" + std::string(resp->getBody())));
                    return;
                }

                Json::CharReaderBuilder builder;
                Json::Value pageJson;
                std::string parseErr;
                std::istringstream iss(std::string(resp->getBody()));
                if (!Json::parseFromStream(builder, iss, &pageJson, &parseErr) || !pageJson.isArray())
                {
                    callback(makeErrorResponse(k502BadGateway, "Invalid GitHub JSON response: " + parseErr));
                    return;
                }

                for (const auto &item : pageJson)
                {
                    state->commits.append(item);
                }

                if (static_cast<int>(pageJson.size()) == state->perPage && page < state->maxPages)
                {
                    (*fetchAndSync)(page + 1);
                    return;
                }

                Json::Value ret;
                try
                {
                    auto trans = state->dbClient->newTransaction();
                    int insertedCount = 0;
                    int updatedCount = 0;
                    int skippedCount = 0;
                    int syncedCount = 0;
                    Json::Value skippedItems(Json::arrayValue);

                    for (const auto &commit : state->commits)
                    {
                        const std::string commitSha = commit["sha"].isNull() ? "" : commit["sha"].asString();
                        std::string htmlUrl = commit["html_url"].isNull() ? "" : commit["html_url"].asString();

                        std::string message = "";
                        std::string committedAt = "";
                        if (!commit["commit"].isNull())
                        {
                            message = commit["commit"]["message"].isNull() ? "" : commit["commit"]["message"].asString();
                            if (!commit["commit"]["author"].isNull())
                            {
                                committedAt = commit["commit"]["author"]["date"].isNull()
                                                ? ""
                                                : commit["commit"]["author"]["date"].asString();
                            }
                            if (committedAt.empty() && !commit["commit"]["committer"].isNull())
                            {
                                committedAt = commit["commit"]["committer"]["date"].isNull()
                                                ? ""
                                                : commit["commit"]["committer"]["date"].asString();
                            }
                        }

                        int64_t githubAuthorId = 0;
                        std::string githubAuthorLogin;
                        if (!commit["author"].isNull())
                        {
                            githubAuthorId = commit["author"]["id"].isNull() ? 0 : commit["author"]["id"].asInt64();
                            githubAuthorLogin = commit["author"]["login"].isNull() ? "" : commit["author"]["login"].asString();
                        }

                        if (githubAuthorLogin.empty() && !commit["committer"].isNull())
                        {
                            githubAuthorId = commit["committer"]["id"].isNull() ? 0 : commit["committer"]["id"].asInt64();
                            githubAuthorLogin = commit["committer"]["login"].isNull() ? "" : commit["committer"]["login"].asString();
                        }

                        if (githubAuthorLogin.empty() && !commit["commit"].isNull() && !commit["commit"]["author"].isNull())
                        {
                            githubAuthorLogin = commit["commit"]["author"]["name"].isNull()
                                                    ? ""
                                                    : commit["commit"]["author"]["name"].asString();
                        }

                        if (githubAuthorLogin.empty() && !commit["commit"].isNull() && !commit["commit"]["author"].isNull())
                        {
                            githubAuthorLogin = commit["commit"]["author"]["email"].isNull()
                                                    ? ""
                                                    : commit["commit"]["author"]["email"].asString();
                        }

                        if (commitSha.empty() || htmlUrl.empty() || committedAt.empty() || githubAuthorLogin.empty())
                        {
                            Json::Value skipped;
                            skipped["commit_sha"] = commitSha;
                            skipped["html_url"] = htmlUrl;

                            Json::Value missingFields(Json::arrayValue);
                            if (commitSha.empty())
                            {
                                missingFields.append("commit_sha");
                            }
                            if (htmlUrl.empty())
                            {
                                missingFields.append("html_url");
                            }
                            if (committedAt.empty())
                            {
                                missingFields.append("committed_at");
                            }
                            if (githubAuthorLogin.empty())
                            {
                                missingFields.append("github_author_login");
                            }

                            skipped["missing_fields"] = missingFields;
                            skipped["reason"] = "Missing required commit fields";
                            skippedItems.append(skipped);
                            ++skippedCount;
                            continue;
                        }

                        message = truncateTo255(message);

                        auto exists = trans->execSqlSync(
                            R"(
                                SELECT id FROM commits
                                WHERE commit_sha = $1 AND github_repository_id = $2
                                LIMIT 1
                            )",
                            commitSha,
                            state->repoId);

                        if (!exists.empty())
                        {
                            const std::string rowId = exists[0]["id"].as<std::string>();
                            trans->execSqlSync(
                                R"(
                                    UPDATE commits
                                    SET html_url = $1,
                                        github_author_id = $2,
                                        github_author_login = $3,
                                        message = $4,
                                        github_repository_name = $5,
                                        committed_at = $6::timestamptz
                                    WHERE id = $7
                                )",
                                htmlUrl,
                                githubAuthorId,
                                githubAuthorLogin,
                                message,
                                state->repo,
                                committedAt,
                                rowId);
                            // trans->execSqlSync(
                            //     R"(
                            //         UPDATE commits
                            //         SET html_url = $1,
                            //             user_id = $2,
                            //             user_name = $3,
                            //             github_author_id = $4,
                            //             github_author_login = $5,
                            //             message = $6,
                            //             github_repository_name = $7,
                            //             committed_at = $8::timestamptz
                            //         WHERE id = $9
                            //     )",
                            //     htmlUrl,
                            //     state->userId,
                            //     state->userName,
                            //     githubAuthorId,
                            //     githubAuthorLogin,
                            //     message,
                            //     state->repo,
                            //     committedAt,
                            //     rowId);
                            ++updatedCount;
                        }
                        else
                        {
                            trans->execSqlSync(
                                R"(
                                    INSERT INTO commits (
                                        html_url,
                                        commit_sha,
                                        github_author_id,
                                        github_author_login,
                                        message,
                                        github_repository_id,
                                        github_repository_name,
                                        committed_at,
                                        project_id,
                                        milestone_id,
                                        task_id
                                    ) VALUES (
                                        $1, $2, $3, $4, $5, $6, $7, $8::timestamptz,
                                        NULL, NULL, NULL
                                    )
                                )",
                                htmlUrl,
                                commitSha,
                                githubAuthorId,
                                githubAuthorLogin,
                                message,
                                state->repoId,
                                state->repo,
                                committedAt);
                            ++insertedCount;
                        }

                        ++syncedCount;
                    }

                    ret["result"] = "ok";
                    ret["fetched_count"] = static_cast<int>(state->commits.size());
                    ret["synced_count"] = syncedCount;
                    ret["inserted_count"] = insertedCount;
                    ret["updated_count"] = updatedCount;
                    ret["skipped_count"] = skippedCount;
                    ret["skipped_items"] = skippedItems;
                }
                catch (const std::exception &e)
                {
                    callback(makeErrorResponse(k500InternalServerError, std::string("Sync failed: ") + e.what()));
                    return;
                }

                callback(HttpResponse::newHttpJsonResponse(ret));
            });
    };

    (*fetchAndSync)(1);
}