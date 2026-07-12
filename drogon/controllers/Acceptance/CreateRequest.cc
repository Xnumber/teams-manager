#include "AcceptanceCtrl.h"
#include <jsoncpp/json/json.h>
#include <drogon/HttpResponse.h>
#include "jwt/jwt.hpp"
#include "sql/sql.h"
#include "plugins/RequestContext/RequestContext.h"
#include "models/PlansProjects.h"
using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::teams_manager;

void AcceptanceCtrl::createRequest(const HttpRequestPtr &req,
								   std::function<void(const HttpResponsePtr &)> &&callback)
{
    LOG_DEBUG << "Acceptance request create called";
	try
	{
        DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
		std::shared_ptr<Json::Value> json = req->getJsonObject();
        std::string user_id = RequestContext::getUserId();
        std::string task_id = (*json)["task_id"].asString();
		
		if (!json)
		{
			Json::Value error;
			error["result"] = "error";
			error["message"] = "Invalid JSON body";
			auto resp = HttpResponse::newHttpJsonResponse(error);
			resp->setStatusCode(k400BadRequest);
			callback(resp);
			return;
		}

		clientPtr->execSqlAsync(
			createTaskAcceptanceRequestSql,
			[callback](const Result &r)
			{
				if (r.empty())
				{
					Json::Value err;
					err["result"] = "error";
					err["message"] = "Create Request failed";
					auto resp = HttpResponse::newHttpJsonResponse(err);
					resp->setStatusCode(k500InternalServerError);
					callback(resp);
					return;
				}

				Json::Value ret;
				ret["result"] = "ok";
				// ret["data"] = Json::Value(Json::objectValue);
				drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(ret);
				callback(resp);
			},
			[callback](const DrogonDbException &e)
			{
				LOG_ERROR << "Acceptance Request Error: " << e.base().what();
				Json::Value err;
				err["result"] = "error";
				err["message"] = std::string("Create failed: ") + e.base().what();
				auto resp = HttpResponse::newHttpJsonResponse(err);
				resp->setStatusCode(k500InternalServerError);
				callback(resp);
			},
            user_id,
			task_id
		);
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