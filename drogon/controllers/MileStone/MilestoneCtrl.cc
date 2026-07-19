#include "MilestoneCtrl.h"
#include <jsoncpp/json/json.h>
#include <drogon/HttpResponse.h>
#include "models/Milestones.h"
#include "utils/mapToJson.h"
#include "jwt/jwt.hpp"
#include "models/TMMapper.h"
using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::teams_manager;

void Milestone::get(const HttpRequestPtr &req,
					std::function<void(const HttpResponsePtr &)> &&callback, std::string milestoneId)
{
	LOG_DEBUG << "Milestone get called";
	DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
	orm::Mapper<Milestones> mapper(clientPtr);
	Mapper<Milestones> tenantMapper(clientPtr);
	try
	{
		Milestones milestone = mapper.findByPrimaryKey(milestoneId);
		Json::Value ret;
		ret["result"] = "ok";
		ret["data"] = milestone.toJson();
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


void Milestone::list(const HttpRequestPtr &req,
					 std::function<void(const HttpResponsePtr &)> &&callback)
{
	jwt::jwt_payload jwtPayload = req->getAttributes()->get<jwt::jwt_payload>("jwt_payload");
	std::string tenant_id = jwtPayload.get_claim_value<std::string>("tenant_id");
	std::string project_id = req->getParameter("project_id");
	std::string filter = req->getParameter("filter");
	std::string completeInput = req->getParameter("completed");
	// LOG_DEBUG << "JWT Tenant_id in Milestone List: " << tenant_id;
	try
	{
		DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
		Mapper<Milestones> mapper(clientPtr);
		std::vector<Milestones> milestoneList;
		Criteria criteria;
		bool hasCompleteFilter = false;
		bool completeValue = false;

		if (!completeInput.empty())
		{
			if (completeInput == "true")
			{
				hasCompleteFilter = true;
				completeValue = true;
			}
			else if (completeInput == "false")
			{
				hasCompleteFilter = true;
				completeValue = false;
			}
			else
			{
				Json::Value error;
				error["result"] = "error";
				error["message"] = "Invalid completed value. Use true/false.";
				auto resp = HttpResponse::newHttpJsonResponse(error);
				resp->setStatusCode(k400BadRequest);
				callback(resp);
				return;
			}
		}

		if (!project_id.empty()) {
			criteria = Criteria(Milestones::Cols::_project_id, CompareOperator::EQ, project_id);
		}

		if(!filter.empty()) {
			criteria = criteria && (Criteria(Milestones::Cols::_name, CompareOperator::Like, "%" + filter + "%") || Criteria(Milestones::Cols::_description, CompareOperator::Like, "%" + filter + "%"));
		}

		if (hasCompleteFilter)
		{
			criteria = criteria && Criteria(Milestones::Cols::_completed, CompareOperator::EQ, completeValue);
		}

		milestoneList = mapper
		.orderBy(Milestones::Cols::_priority, SortOrder::ASC)
		.findBy(criteria);
		
		Json::Value ret;
		ret["result"] = "ok";
		ret["data"] = mapToJson(milestoneList, [](const auto &m)
								{ return m.toJson(); });
		ret["count"] = static_cast<int>(milestoneList.size());
		drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(ret);
		callback(resp);
	}
	catch (const DrogonDbException &e)
	{
		LOG_ERROR << "Database error: " << e.base().what();
		Json::Value error;
		error["result"] = "error";
		error["message"] = e.base().what();
		auto resp = HttpResponse::newHttpJsonResponse(error);
		resp->setStatusCode(k500InternalServerError);
		callback(resp);
	}
}

void Milestone::remove(const HttpRequestPtr &req,
					   std::function<void(const HttpResponsePtr &)> &&callback, std::string milestoneId)
{
	LOG_DEBUG << "Milestone remove called";
	DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
	orm::Mapper<Milestones> mapper(clientPtr);
	try
	{
		Milestones milestone = mapper.findByPrimaryKey(milestoneId);
		mapper.deleteByPrimaryKey(milestoneId);
		Json::Value ret;
		ret["result"] = "ok";
		ret["data"] = milestone.toJson();
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
