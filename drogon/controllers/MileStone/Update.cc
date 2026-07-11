#include "MilestoneCtrl.h"
#include <jsoncpp/json/json.h>
#include <drogon/HttpResponse.h>
#include "models/Milestones.h"
#include "utils/mapToJson.h"
#include "jwt/jwt.hpp"
#include "models/TMMapper.h"
#include "sql/update.h"
using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::teams_manager;

std::string Milestone::getString(const Json::Value &json, const char *key)
{
	if (!json.isMember(key) || json[key].isNull())
		return "";
	return json[key].asString();
}

std::string Milestone::getIntAsString(const Json::Value &json, const char *key)
{
	if (!json.isMember(key) || json[key].isNull())
		return "";
	return std::to_string(json[key].asInt());
}

std::string Milestone::getBoolAsString(const Json::Value &json, const char *key)
{
	if (!json.isMember(key) || json[key].isNull())
		return "";
	return json[key].asBool() ? "true" : "false";
}

void Milestone::update(const HttpRequestPtr &req,
					   std::function<void(const HttpResponsePtr &)> &&callback, std::string milestoneId)
{
	LOG_DEBUG << "Milestone update called";
	DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
	orm::Mapper<Milestones> mapper(clientPtr);
	try
	{
		auto json = req->getJsonObject();
		if (!json)
		{
			Json::Value error;
			error["result"] = "error";
			error["message"] = "Invalid JSON body";
			drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(error);
			resp->setStatusCode(k400BadRequest);
			callback(resp);
			return;
		}
		Milestones milestone = mapper.findByPrimaryKey(milestoneId);
		std::string db_stamp_opt = *milestone.getConcurrencyStamp();
		std::string req_stamp = (*json)["concurrency_stamp"].asString();
		if (req_stamp.empty() || req_stamp != db_stamp_opt)
		{
			Json::Value error;
			error["result"] = "error";
			error["message"] = "Concurrency stamp mismatch";
			drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(error);
			resp->setStatusCode(k400BadRequest);
			callback(resp);
			return;
		}

		std::string newConcurrencyStamp = drogon::utils::getUuid();

		clientPtr->execSqlAsync(
			getMilestoneUpdateSql,
			[callback](const Result &r)
			{
				if (r.empty())
				{
					Json::Value err;
					err["result"] = "error";
					err["message"] = "Update milestone failed";
					auto resp = HttpResponse::newHttpJsonResponse(err);
					resp->setStatusCode(k500InternalServerError);
					callback(resp);
					return;
				}

				Milestones updatedMilestone(r[0], -1);

				Json::Value ret;
				ret["result"] = "ok";
				ret["data"] = updatedMilestone.toJson();
				drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(ret);
				callback(resp);
			},
			[callback](const DrogonDbException &e)
			{
				LOG_ERROR << "Milestone update error: " << e.base().what();
				Json::Value err;
				err["result"] = "error";
				err["message"] = std::string("Update failed: ") + e.base().what();
				auto resp = HttpResponse::newHttpJsonResponse(err);
				resp->setStatusCode(k500InternalServerError);
				callback(resp);
			},
			milestoneId,
			getString(*json, "name"),
			getString(*json, "description"),
			getString(*json, "project_id"),
			getString(*json, "project_name"),
			getString(*json, "team_id"),
			getString(*json, "team_name"),
			getString(*json, "tenant_id"),
			getString(*json, "tenant_name"),
			getString(*json, "start_date"),
			getString(*json, "end_date"),
			getString(*json, "creator_id"),
			getString(*json, "creator_name"),
			newConcurrencyStamp,
			getString(*json, "remark"),
			"",
			getIntAsString(*json, "estimated_remaining_workdays"),
			getString(*json, "dependency_milestone_id"),
			getString(*json, "estimated_start_date"),
			getString(*json, "estimated_end_date"),
			getBoolAsString(*json, "completed"),
			getBoolAsString(*json, "is_for_demo")
		);
	}
	catch (const std::exception &e)
	{
		LOG_ERROR << "Error: " << e.what();
		Json::Value error;
		error["result"] = "error";
		error["message"] = e.what();
		drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(error);
		resp->setStatusCode(k500InternalServerError);
		callback(resp);
	}
}
