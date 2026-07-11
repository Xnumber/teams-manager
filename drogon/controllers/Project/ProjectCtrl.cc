#include "ProjectCtrl.h"
#include <jsoncpp/json/json.h>
#include <drogon/HttpResponse.h>
#include "models/Projects.h"
#include "jwt/jwt.hpp"
#include "sql/list.h"
#include "sql/sql.h"
// #include "../plugins/RequestContext/RequestContext.h"


// #include "../tenant_example/solution1_wrapper/Mapper.h"
#include "models/TMMapper.h"
using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::teams_manager;


void Project::get(const HttpRequestPtr &req,
				  std::function<void(const HttpResponsePtr &)> &&callback, std::string projectId)
{
	// std::string tenant_id = RequestContext::getTenantId();
	LOG_DEBUG << "Project get called";
	// LOG_DEBUG << "Tenant ID: " << tenant_id;
	DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
	orm::Mapper<Projects> mapper(clientPtr);
	Mapper<Projects> tenantMapper(clientPtr); // tenantId will be fetched from RequestContext inside Mapper
	try
	{
		Projects project = mapper.findByPrimaryKey(projectId);
		Json::Value ret;
		ret["result"] = "ok";
		ret["data"] = project.toJson();
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





void Project::list(const HttpRequestPtr &req,
				   std::function<void(const HttpResponsePtr &)> &&callback)
{
	// LOG_DEBUG << "Project list called";
	// std::string tenant_id_2 = RequestContext::getTenantId();
	// LOG_DEBUG << "Tenant ID 2: " << tenant_id_2;
	jwt::jwt_payload jwtPayload = req->getAttributes()->get<jwt::jwt_payload>("jwt_payload");
	// std::string tenant_id = req->getAttributes()->get<std::string>("tenant_id");
	// std::cout << "JWT Payload in Project List: " << jwtPayload << std::endl;
	std::string tenant_id = jwtPayload.get_claim_value<std::string>("tenant_id");
	LOG_DEBUG << "JWT Tenant_id in Project List: " << tenant_id;
	// jwt::jwt_object jwtPayload = req->getAttributes()->get<jwt::jwt_payload>("jwt_payload");
	// jwt::jwt_object jwtPayload = req->attributes()->get("f");
	LOG_DEBUG << "Project list called";

	std::string filter = req->getParameter("filter");
	try
	{
		DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
		// orm::Mapper<Projects> mapper(clientPtr);
		Mapper<Projects> mapper(clientPtr);
		// Mapper<Projects> tenantMapper(clientPtr);
		// std::vector<Projects> projectList = mapper.findBy(Criteria("tenant_id", CompareOperator::EQ, tenant_id));
		clientPtr->execSqlAsync(
			getProjectsListSql,
			[callback](const drogon::orm::Result &r) {
				Json::Value projects(Json::arrayValue);
				for (const auto &row : r)
				{
					Json::Value item;
					item["name"] = row["name"].as<std::string>();
					item["priority"] = row["priority"].as<int>();
					item["description"] = row["description"].isNull() ? "" : row["description"].as<std::string>();
					item["milestones_count_in_queue"] = row["milestones_count_in_queue"].as<int>();
					item["tasks_count_in_queue"] = row["tasks_count_in_queue"].as<int>();
					item["id"] = row["id"].as<std::string>();
					item["concurrency_stamp"] = row["concurrency_stamp"].as<std::string>();
					item["created_at"] = row["created_at"].as<std::string>();
					item["tenant_id"] = row["tenant_id"].as<std::string>();
					projects.append(item);
				}
				Json::Value ret;
				ret["result"] = "ok";
				ret["data"] = projects;
				ret["count"] = static_cast<int>(projects.size());
				drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(ret);
				callback(resp);
			},
			[callback](const DrogonDbException &e) {
				LOG_ERROR << "Database error: " << e.base().what();
				Json::Value error;
				error["result"] = "error";
				error["message"] = e.base().what();
				drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(error);
				resp->setStatusCode(k500InternalServerError);
				callback(resp);
			},
			filter
		);
		// Json::Value ret;
		// ret["result"] = "ok";
		// ret["projects"] = mapToJson(projectList, [](const auto &p) { return p.toJson(); });
		// ret["count"] = static_cast<int>(projectList.size());
		// drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(ret);
		// callback(resp);
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

void Project::listForAnalytics(const HttpRequestPtr &req,
					   std::function<void(const HttpResponsePtr &)> &&callback)
{
	list(req, std::move(callback));
}

void Project::create(const HttpRequestPtr &req,
					 std::function<void(const HttpResponsePtr &)> &&callback)
{
	jwt::jwt_payload jwtPayload = req->getAttributes()->get<jwt::jwt_payload>("jwt_payload");
	// std::cout << "JWT Payload in Project Create: " << jwtPayload << std::endl;
	// std::string tenant_id = jwtPayload.get_claim_value<std::string>("tenant_id");
	std::string tenant_id = RequestContext::getTenantId();
	
	
	LOG_DEBUG << "Project create called";
	try
	{
		DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
        std::shared_ptr<Json::Value> json = req->getJsonObject();
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

		std::string name = (*json)["name"].asString();
		std::string description = (*json).isMember("description") ? (*json)["description"].asString() : "";
		std::string creator_id = RequestContext::getUserId();
		std::string concurrency_stamp = drogon::utils::getUuid();
		std::string estimated_remaining_workdays = (*json).isMember("estimated_remaining_workdays")
													 ? std::to_string((*json)["estimated_remaining_workdays"].asInt())
													 : "0";
		// std::string priority = (*json).isMember("priority")
		// 						? std::to_string((*json)["priority"].asInt())
		// 						: "0";

		Json::Value dependencies = (*json).isMember("dependencies") ? (*json)["dependencies"] : Json::Value(Json::arrayValue);

		std::cout << "Received JSON for creating project: " << (*json).toStyledString() << std::endl;
		std::cout << tenant_id << std::endl;
		std::cout << concurrency_stamp << std::endl;

		clientPtr->execSqlAsync(
			createProjectSql,
			[callback](const Result &r)
			{
				if (r.empty())
				{
					Json::Value err;
					err["result"] = "error";
					err["message"] = "Create project failed";
					auto resp = HttpResponse::newHttpJsonResponse(err);
					resp->setStatusCode(k500InternalServerError);
					callback(resp);
					return;
				}

				Projects newProject(r[0], -1);

				Json::Value ret;
				ret["result"] = "ok";
				ret["data"] = newProject.toJson();
				auto resp = HttpResponse::newHttpJsonResponse(ret);
				callback(resp);
			},
			[callback](const DrogonDbException &e)
			{
				LOG_ERROR << "Project create error: " << e.base().what();
				Json::Value err;
				err["result"] = "error";
				err["message"] = std::string("Create failed: ") + e.base().what();
				auto resp = HttpResponse::newHttpJsonResponse(err);
				resp->setStatusCode(k500InternalServerError);
				callback(resp);
			},
			tenant_id,
			name,
			description,
			creator_id,
			concurrency_stamp,
			estimated_remaining_workdays,
			dependencies.toStyledString()
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

void Project::update(const HttpRequestPtr &req,
					 std::function<void(const HttpResponsePtr &)> &&callback, std::string projectId)
{
	LOG_DEBUG << "Project update called";
	DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
	orm::Mapper<Projects> mapper(clientPtr);
	try
	{
		auto json = req->getJsonObject();
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

        
		Projects project = mapper.findByPrimaryKey(projectId);
		// 檢查 concurrency_stamp
		std::string db_stamp_opt = *project.getConcurrencyStamp();
		std::string req_stamp = (*json)["concurrency_stamp"].asString();

        if (req_stamp.empty() || req_stamp != db_stamp_opt)
        {
            Json::Value error;
            error["result"] = "error";
            error["message"] = "Concurrency stamp mismatch";
            auto resp = HttpResponse::newHttpJsonResponse(error);
            resp->setStatusCode(k400BadRequest);
            callback(resp);
            return;
        }
		project.updateByJson(*json);
		project.setConcurrencyStamp(drogon::utils::getUuid()); // 若有此欄位可加
		mapper.update(project);
		Json::Value ret;
		ret["result"] = "ok";
		ret["data"] = project.toJson();
		// LOG_DEBUG << "Updated project: " << ret.toStyledString();
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

void Project::remove(const HttpRequestPtr &req,
					 std::function<void(const HttpResponsePtr &)> &&callback, std::string projectId)
{
	LOG_DEBUG << "Project remove called";
	DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
	orm::Mapper<Projects> mapper(clientPtr);
	try
	{
		Projects project = mapper.findByPrimaryKey(projectId);
		mapper.deleteByPrimaryKey(projectId);
		Json::Value ret;
		ret["result"] = "ok";
		ret["project"] = project.toJson();
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