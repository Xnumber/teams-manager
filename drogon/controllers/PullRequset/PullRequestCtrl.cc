#include "PullRequestCtrl.h"
#include <drogon/HttpClient.h>
#include <drogon/HttpResponse.h>
#include <jsoncpp/json/json.h>
#include <algorithm>
#include <functional>
#include <memory>
#include <sstream>
#include <string>
#include "models/Repositories.h"
#include "models/PullRequestSyncCheckpoints.h"
#include "utils/HttpErrorHandler.h"

using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::teams_manager;

// Add definition of your processing function here
namespace
{
struct SyncState
{
	DbClientPtr dbClient;
	HttpClientPtr ghClient;
	std::optional<int64_t> userId;
	std::string owner;
	std::string repo;
	std::string startAt;
	std::string endAt;
	std::string state;
	std::string sort;
	std::string direction;
	bool hasCheckpoint{false};
	std::string checkpoint;
	int64_t checkpointPrId{0};
	std::string maxUpdatedAt;
	int64_t maxUpdatedPrId{0};
	int64_t repoId{0};
	int perPage{100};
	int maxPages{10};
	std::string token;
	Json::Value pulls{Json::arrayValue};
};

bool inRange(const std::string &value, const std::string &startAt, const std::string &endAt)
{
	if (value.empty())
	{
		return false;
	}
	return value >= startAt && value <= endAt;
}
} // namespace

void PullRequestCtrl::syncPullRequests(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
{
	auto json = req->getJsonObject();
	if (!json)
	{
		callback(makeErrorResponse(k400BadRequest, "Invalid JSON body"));
		return;
	}


	// 新版：只接受 repo_id 與 github_token
	if (!(*json).isMember("repo_id")) {
		callback(makeErrorResponse(k400BadRequest, "Missing required field: repo_id"));
		return;
	}
	int64_t repo_id = (*json)["repo_id"].asInt64();
	std::string github_token;
	if ((*json).isMember("github_token")) {
		github_token = (*json)["github_token"].asString();
	} else {
		callback(makeErrorResponse(k400BadRequest, "Missing required field: github_token"));
		return;
	}

	// 查詢 repositories 取得 owner/repo_name
	DbClientPtr dbClient = drogon::app().getDbClient("teams_manager");
	orm::Mapper<Repositories> repoMapper(dbClient);
	orm::Mapper<PullRequestSyncCheckpoints> checkPointMapper(dbClient);

	Repositories repoObj;
	std::string dbCheckpoint;
	int64_t dbCheckpointPrId = 0;

	try {
		repoObj = repoMapper.findByPrimaryKey(repo_id);
		try {
			auto checkpointObj = checkPointMapper.findByPrimaryKey(repo_id);
			if (checkpointObj.getLastUpdatedAt()) {
				dbCheckpoint = checkpointObj.getValueOfLastUpdatedAt().toCustomedFormattedString("%Y-%m-%dT%H:%M:%SZ");
			}
			if (checkpointObj.getLastPrId()) {
				dbCheckpointPrId = checkpointObj.getValueOfLastPrId();
			}
		} catch (const drogon::orm::UnexpectedRows &) {
			// checkpoint 尚不存在，後續會在同步完成後建立
		}
	} catch (const std::exception &e) {
		callback(makeErrorResponse(k404NotFound, std::string("Repository not found: ") + e.what()));
		return;
	}

	auto state = std::make_shared<SyncState>();
	state->dbClient = dbClient;
	state->ghClient = HttpClient::newHttpClient("https://api.github.com");
	state->owner = repoObj.getValueOfOwner();
	state->repo = repoObj.getValueOfRepoName();
	state->token = github_token;
	state->repoId = repo_id;
	state->state = (*json).isMember("state") ? (*json)["state"].asString() : "all";
	state->sort = (*json).isMember("sort") ? (*json)["sort"].asString() : "updated";
	state->direction = (*json).isMember("direction") ? (*json)["direction"].asString() : "desc";
	const bool hasRequestCheckpoint = (*json).isMember("checkpoint") && !(*json)["checkpoint"].asString().empty();
	state->checkpoint = hasRequestCheckpoint ? (*json)["checkpoint"].asString() : dbCheckpoint;
	state->checkpointPrId = hasRequestCheckpoint ? 0 : dbCheckpointPrId;
	state->hasCheckpoint = !state->checkpoint.empty();
	// state->perPage = (*json).isMember("per_page") ? std::clamp((*json)["per_page"].asInt(), 1, 100) : 100;
	state->perPage = 100;
	state->maxPages = (*json).isMember("max_pages") ? std::clamp((*json)["max_pages"].asInt(), 1, 50) : 10;

	std::shared_ptr<trantor::SockOptCallback> fetchAndSync = std::make_shared<std::function<void(int)>>();
	*fetchAndSync = [state, callback, fetchAndSync](int page) {
		auto ghReq = HttpRequest::newHttpRequest();
		ghReq->setMethod(Get);
        ghReq->setPath("/repos/" + state->owner + "/" + state->repo +
            "/pulls?" +
			"state=" + state->state +
			"&sort=" + state->sort +
			"&direction=" + state->direction +
            "&per_page=" + std::to_string(state->perPage) +
            "&page=" + std::to_string(page));
		ghReq->addHeader("Accept", "application/vnd.github+json");
		ghReq->addHeader("X-GitHub-Api-Version", "2026-03-10");

		if (!state->token.empty())
		{
			ghReq->addHeader("Authorization", "Bearer " + state->token);
		}

        const std::string authHeader = ghReq->getHeader("Authorization");
        const std::string url = ghReq->getPath();
        const drogon::SafeStringMap<std::string> params = ghReq->getParameters();
        std::cout << "Authorization header: " << authHeader << std::endl;
        std::cout << "Request URL: " << url << std::endl;
        // ghReq->getAttribute
		state->ghClient->sendRequest(
			ghReq,
			[state, callback, fetchAndSync, page](ReqResult result, const HttpResponsePtr &resp) {
                std::cout << "GitHub API response status: " << (resp ? std::to_string(static_cast<int>(resp->statusCode())) : "null") << std::endl;
                std::cout << "Syncing Page: " << page << std::endl;
				if (result != ReqResult::Ok || !resp)
				{
					callback(makeErrorResponse(k502BadGateway, "Failed to request GitHub API"));
					return;
				}

				if (resp->statusCode() == k304NotModified)
				{
					Json::Value ret;
					ret["result"] = "ok";
					ret["fetched_count"] = 0;
					ret["synced_count"] = 0;
					ret["inserted_count"] = 0;
					ret["updated_count"] = 0;
					ret["skipped_count"] = 0;
					callback(HttpResponse::newHttpJsonResponse(ret));
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
				std::cout << "Fetched pageJson" << pageJson.size() << " PRs from GitHub API (Page " << page << ")" << std::endl;
				bool stopPaginationByCheckpoint = false;
				for (const auto &item : pageJson)
				{
					const int64_t prNumber = item["number"].isNull() ? 0 : item["number"].asInt64();
					const std::string updatedAt = item["updated_at"].isNull() ? "" : item["updated_at"].asString();
					if (state->hasCheckpoint && !updatedAt.empty() && updatedAt < state->checkpoint)
					{
						stopPaginationByCheckpoint = true;
						break;
					}
					if (state->hasCheckpoint && !updatedAt.empty() && updatedAt == state->checkpoint &&
						state->checkpointPrId > 0 && prNumber <= state->checkpointPrId)
					{
						stopPaginationByCheckpoint = true;
						break;
					}

					if (!updatedAt.empty() && updatedAt > state->maxUpdatedAt)
					{
						state->maxUpdatedAt = updatedAt;
						state->maxUpdatedPrId = prNumber;
					}
					else if (!updatedAt.empty() && updatedAt == state->maxUpdatedAt && prNumber > state->maxUpdatedPrId)
					{
						state->maxUpdatedPrId = prNumber;
					}

					std::cout << "Fetched PR #" << item["number"].asInt() << ": " << item["title"].asString() << std::endl;
					state->pulls.append(item);
				}

				// if (!stopPaginationByCheckpoint && static_cast<int>(pageJson.size()) == state->perPage && page < state->maxPages)
				if (!stopPaginationByCheckpoint && static_cast<int>(pageJson.size()) == state->perPage)
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
					std::cout << "Total PRs fetched: " << state->pulls.size() << std::endl;
					for (const auto &pr : state->pulls)
					{
						std::cout << "Processing PR #" << pr["number"].asInt() << ": " << pr["title"].asString() << std::endl;
						const int prNumber = pr["number"].asInt();
						const int64_t prUserId = pr["user"]["id"].asInt64();
						const std::string prUserLogin = pr["user"]["login"].asString();
						const std::string htmlUrl = pr["html_url"].isNull() ? "" : pr["html_url"].asString();
						const std::string title = pr["title"].isNull() ? "" : pr["title"].asString();
						const std::string body = pr["body"].isNull() ? "" : pr["body"].asString();
						const std::string prState = pr["state"].isNull() ? "" : pr["state"].asString();
                        const std::string createdAt = pr["created_at"].isNull() ? "" : pr["created_at"].asString();
						const std::string mergedAt = pr["merged_at"].isNull() ? "" : pr["merged_at"].asString();
						const bool isMerged = !mergedAt.empty();
                        const std::string updatedAt = pr["updated_at"].isNull() ? "" : pr["updated_at"].asString();
                        const std::string closedAt = pr["closed_at"].isNull() ? "" : pr["closed_at"].asString();

						int64_t repoId = 0;
						std::string repoName;
						if (!pr["base"].isNull() && !pr["base"]["repo"].isNull())
						{
							repoId = pr["base"]["repo"]["id"].isNull() ? 0 : pr["base"]["repo"]["id"].asInt64();
							repoName = pr["base"]["repo"]["name"].isNull() ? state->repo : pr["base"]["repo"]["name"].asString();
						}
						if (repoId == 0)
						{
							++skippedCount;
							continue;
						}


						drogon::orm::Result exists = trans->execSqlSync(R"(
							SELECT id FROM pull_requests
							WHERE pr_number =$1 AND github_repository_id =$2
							LIMIT 1
						)",
						prNumber, repoId);

						trans->execSqlSync(
							R"(
							INSERT INTO pull_requests (
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
							) VALUES (
								$1, $2, $3, $4, $5, $6, $7, $8, $9, $10, $11,
								$12::timestamptz,
								NULLIF($13, '')::timestamptz,
								NULLIF($14, '')::timestamptz,
								NULLIF($15, '')::timestamptz,
								NULL, NULL, NULL
							)
							ON CONFLICT (pr_number, github_repository_id)
							DO UPDATE SET
								pr_user_id=EXCLUDED.pr_user_id,
								pr_user_login=EXCLUDED.pr_user_login,
								html_url=EXCLUDED.html_url,
								title=EXCLUDED.title,
								body=EXCLUDED.body,
								state=EXCLUDED.state,
								is_merged=EXCLUDED.is_merged,
								github_repository_name=EXCLUDED.github_repository_name,
								created_at=EXCLUDED.created_at,
								updated_at=EXCLUDED.updated_at,
								closed_at=EXCLUDED.closed_at,
								merged_at=EXCLUDED.merged_at
							)",
							state->userId,
							prNumber,
							prUserId,
							prUserLogin,
							htmlUrl,
							title,
							body,
							prState,
							isMerged,
							repoId,
							repoName,
							createdAt,
							updatedAt,
							closedAt,
							mergedAt);
						// 判斷是否為 insert 或 update
						if (!exists.empty()) {
							std::cout << "Updated PR #" << prNumber << " in database." << std::endl;
							++updatedCount;
						} else {
							std::cout << "Inserted PR #" << prNumber << " into database." << std::endl;
							++insertedCount;
						}

						++syncedCount;
					}

					ret["result"] = "ok";
					ret["fetched_count"] = static_cast<int>(state->pulls.size());
					ret["synced_count"] = syncedCount;
					ret["inserted_count"] = insertedCount;
					ret["updated_count"] = updatedCount;
					ret["skipped_count"] = skippedCount;
					ret["checkpoint"] = state->checkpoint;
					ret["next_checkpoint"] = state->maxUpdatedAt;

					// 更新 pull_request_sync_checkpoints（upsert）
					if (!state->maxUpdatedAt.empty()) {
						trans->execSqlSync(
							R"(INSERT INTO pull_request_sync_checkpoints (repo_id, repo_name, last_updated_at, last_pr_id)
							VALUES ($1, $2, $3::timestamptz, $4)
							ON CONFLICT (repo_id) DO UPDATE
							SET repo_name = EXCLUDED.repo_name,
							    last_updated_at = EXCLUDED.last_updated_at,
							    last_pr_id = EXCLUDED.last_pr_id)",
							state->repoId,
							state->repo,
							state->maxUpdatedAt,
							state->maxUpdatedPrId);
					}
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