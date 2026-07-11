#include "RepositoryCtrl.h"
#include <jsoncpp/json/json.h>
#include <drogon/HttpResponse.h>
#include "models/Repositories.h"
#include "input.h"
#include <drogon/HttpClient.h>
#include "GithubResponse.h"
using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::teams_manager;
void RepositoryCtrl::get(const HttpRequestPtr &req,
                         std::function<void(const HttpResponsePtr &)> &&callback, std::string id)
{
    LOG_DEBUG << "Repository get called";
    DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
    orm::Mapper<Repositories> mapper(clientPtr);
    try
    {
        // 由於 PK 有兩個欄位，這裡假設 id 格式為 "uuid:repo_id"
        size_t pos = id.find(":");
        if (pos == std::string::npos)
            throw std::runtime_error("Invalid id format, should be 'uuid:repo_id'");
        std::string uuid = id.substr(0, pos);
        int64_t repo_id = std::stoll(id.substr(pos + 1));
        Repositories repo = mapper.findByPrimaryKey(repo_id);
        Json::Value ret;
        ret["result"] = "ok";
        ret["data"] = repo.toJson();
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        callback(resp);
    }
    catch (const std::exception &e)
    {
        LOG_ERROR << "Error: " << e.what();
        Json::Value error;
        error["result"] = "error";
        error["message"] = e.what();
        auto resp = HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(k500InternalServerError);
        callback(resp);
    }
}

void RepositoryCtrl::list(const HttpRequestPtr &req,
                          std::function<void(const HttpResponsePtr &)> &&callback)
{
    LOG_DEBUG << "Repository list called";
    DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
    orm::Mapper<Repositories> mapper(clientPtr);
    try
    {
        std::vector<Repositories> repos = mapper.findAll();
        Json::Value ret;
        ret["result"] = "ok";
        for (const auto &repo : repos)
        {
            ret["data"].append(repo.toJson());
        }
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        callback(resp);
    }
    catch (const std::exception &e)
    {
        LOG_ERROR << "Error: " << e.what();
        Json::Value error;
        error["result"] = "error";
        error["message"] = e.what();
        auto resp = HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(k500InternalServerError);
        callback(resp);
    }
}
void RepositoryCtrl::create(const HttpRequestPtr &req,
                            std::function<void(const HttpResponsePtr &)> &&callback)
{
    LOG_DEBUG << "Repository create called";

    auto json = req->getJsonObject();
    Json::Value ret;
    if (!json)
    {
        ret["result"] = "error";
        ret["message"] = "Invalid JSON";
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(k400BadRequest);
        callback(resp);
        return;
    }

    if (!json->isMember("owner") || !(*json)["owner"].isString() ||
        !json->isMember("repo_name") || !(*json)["repo_name"].isString())
    {
        ret["result"] = "error";
        ret["message"] = "owner and repo are required";
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(k400BadRequest);
        callback(resp);
        return;
    }

    RepositoryInput input;
    input.owner = (*json)["owner"].asString();
    input.repo_name = (*json)["repo_name"].asString();
    if (json->isMember("github_pat") && (*json)["github_pat"].isString())
    {
        input.github_pat = (*json)["github_pat"].asString();
    }

    auto client = HttpClient::newHttpClient("https://api.github.com");
    auto githubReq = HttpRequest::newHttpRequest();
    githubReq->setMethod(Get);
    githubReq->setPath("/repos/" + input.owner + "/" + input.repo_name);
    githubReq->addHeader("Accept", "application/vnd.github+json");
    // githubReq->addHeader("User-Agent", "teams-manager");
    if (!input.github_pat.empty())
    {
        githubReq->addHeader("Authorization", "Bearer " + input.github_pat);
    }

    client->sendRequest(githubReq, [callback = std::move(callback), input](ReqResult reqResult, const HttpResponsePtr &githubResp) mutable
                        {
			Json::Value ret;
			if (reqResult != ReqResult::Ok || !githubResp)
			{
				ret["result"] = "error";
				ret["message"] = "GitHub API request failed";
				auto resp = HttpResponse::newHttpJsonResponse(ret);
				resp->setStatusCode(k502BadGateway);
				callback(resp);
				return;
			}

			if (githubResp->statusCode() != k200OK)
			{
				ret["result"] = "error";
				ret["message"] = "GitHub API returned non-200 status";
				ret["github_status"] = static_cast<int>(githubResp->statusCode());
				auto resp = HttpResponse::newHttpJsonResponse(ret);
				resp->setStatusCode(k400BadRequest);
				callback(resp);
				return;
			}

			const std::shared_ptr<Json::Value> githubJson = githubResp->getJsonObject();
			if (!githubJson || !githubJson->isMember("id") || !(*githubJson)["id"].isInt64())
			{
				ret["result"] = "error";
				ret["message"] = "Cannot get repository id from GitHub response";
				auto resp = HttpResponse::newHttpJsonResponse(ret);
				resp->setStatusCode(k400BadRequest);
				callback(resp);
				return;
			}

            std::cout << "GitHub repository found: " << (*githubJson)["full_name"].asString() << " (ID: " << (*githubJson)["id"].asInt64() << ")" << std::endl;

			try
			{
				DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
				orm::Mapper<Repositories> mapper(clientPtr);
				Repositories newRepo;
                newRepo.setOwner(input.owner);
				newRepo.setRepoId((*githubJson)["id"].asInt64());
				newRepo.setRepoName(input.repo_name);
				newRepo.setCreatedAt(trantor::Date::now());
				mapper.insert(newRepo);

				ret["result"] = "ok";
				ret["data"] = newRepo.toJson();
				auto resp = HttpResponse::newHttpJsonResponse(ret);
				callback(resp);
			}
			catch (const std::exception &e)
			{
				LOG_ERROR << "Error: " << e.what();
				ret["result"] = "error";
				ret["message"] = e.what();
				auto resp = HttpResponse::newHttpJsonResponse(ret);
				resp->setStatusCode(k500InternalServerError);
				callback(resp);
			} });
}
