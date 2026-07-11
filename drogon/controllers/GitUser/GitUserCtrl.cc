#include "GitUserCtrl.h"
#include "plugins/RequestContext/RequestContext.h"
#include <drogon/HttpResponse.h>
#include <drogon/HttpClient.h>
#include <jsoncpp/json/json.h>
#include <drogon/orm/DbClient.h>
#include <drogon/orm/Mapper.h>
#include "models/GitUserLinks.h"
#include <string>
using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::teams_manager;

void GitUserCtrl::bindGithubUser(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback) {
	// 1. 取得 pat (從 body)
	auto json = req->getJsonObject();
	// std::string github_login_username;
	std::string pat;
	// if (json && (*json).isMember("github_login_username")) {
	if (json && (*json).isMember("pat")) {
		pat = (*json)["pat"].asString();
	}
	if (pat.empty()) {
		Json::Value err;
		err["result"] = "error";
		err["message"] = "Missing pat in body";
		auto resp = HttpResponse::newHttpJsonResponse(err);
		resp->setStatusCode(k401Unauthorized);
		callback(resp);
		return;
	}

	// 2. 呼叫 github /user
	auto client = HttpClient::newHttpClient("https://api.github.com");
	auto githubReq = HttpRequest::newHttpRequest();
	githubReq->setMethod(Get);
	// githubReq->setPath("/users/" + github_login_username); // Use github_login_username to bypass drogon cache, since github user info won't change frequently. In production, consider using a better cache key strategy.);
	githubReq->setPath("/user"); // Use github_login_username to bypass drogon cache, since github user info won't change frequently. In production, consider using a better cache key strategy.);
	githubReq->addHeader("Authorization", "Bearer " + pat);
	// githubReq->addHeader("Accept", "application/vnd.github+json");
	// githubReq->addHeader("X-GitHub-Api-Version", "2022-11-28");

    
    
    // 4. 取得本地 userId
    std::string userId = RequestContext::getUserId();








	client->sendRequest(githubReq, [callback, req, userId](ReqResult result, const HttpResponsePtr &resp) {
		if (result != ReqResult::Ok || !resp || resp->statusCode() != k200OK) {
			Json::Value err;
			err["result"] = "error";
			err["message"] = "Failed to fetch github user";
			auto r = HttpResponse::newHttpJsonResponse(err);
			r->setStatusCode(k502BadGateway);
			callback(r);
			return;
		}
		// 3. parse github user json
		Json::CharReaderBuilder builder;
		Json::Value userJson;
		std::string parseErr;
		std::istringstream iss(std::string(resp->getBody()));
		if (!Json::parseFromStream(builder, iss, &userJson, &parseErr)) {
			Json::Value err;
			err["result"] = "error";
			err["message"] = "Invalid github user json: " + parseErr;
			auto r = HttpResponse::newHttpJsonResponse(err);
			r->setStatusCode(k502BadGateway);
			callback(r);
			return;
		}
		if (!userJson.isMember("id") || !userJson.isMember("login")) {
			Json::Value err;
			err["result"] = "error";
			err["message"] = "github user json missing id or login";
			auto r = HttpResponse::newHttpJsonResponse(err);
			r->setStatusCode(k502BadGateway);
			callback(r);
			return;
		}
		int64_t githubUserId = userJson["id"].asInt64();
		std::string githubLogin = userJson["login"].asString();

		
		if (userId.empty()) {
			Json::Value err;
			err["result"] = "error";
			err["message"] = "No userId in context";
			auto r = HttpResponse::newHttpJsonResponse(err);
			r->setStatusCode(k401Unauthorized);
			callback(r);
			return;
		}

		// 5. upsert git_user_links
		try {
			drogon::orm::DbClientPtr dbClient = drogon::app().getDbClient("teams_manager");
            orm::Mapper<GitUserLinks> mapper(dbClient);
            GitUserLinks newGitUserLinks;
            newGitUserLinks.setGitUserId(githubUserId);
            newGitUserLinks.setGitUserLogin(githubLogin);
            newGitUserLinks.setUserId(userId);
		    mapper.insert(newGitUserLinks);
		} catch (const std::exception &e) {
			Json::Value err;
			err["result"] = "error";
			err["message"] = std::string("DB error: ") + e.what();
			auto r = HttpResponse::newHttpJsonResponse(err);
			r->setStatusCode(k500InternalServerError);
			callback(r);
			return;
		}

		// 6. 回傳 github user json
		userJson["result"] = "ok";
		callback(HttpResponse::newHttpJsonResponse(userJson));
	});
}
