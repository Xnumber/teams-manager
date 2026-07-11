#include "RoleCtrl.h"
#include <jsoncpp/json/json.h>
#include <drogon/HttpResponse.h>
#include "models/Roles.h"
#include "utils/mapToJson.h"
using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::teams_manager;

void RoleCtrl::get(const HttpRequestPtr &req,
				   std::function<void(const HttpResponsePtr &)> &&callback, std::string roleId)
{
	LOG_DEBUG << "Role get called";
	DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
	orm::Mapper<Roles> mapper(clientPtr);
	try
	{
		Roles role = mapper.findByPrimaryKey(roleId);
		Json::Value ret;
		ret["result"] = "ok";
		ret["data"] = role.toJson();
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

void RoleCtrl::list(const HttpRequestPtr &req,
					std::function<void(const HttpResponsePtr &)> &&callback)
{
	LOG_DEBUG << "Role list called";
	try
	{
		DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
		orm::Mapper<Roles> mapper(clientPtr);
		std::vector<Roles> roleList = mapper.findAll();
		Json::Value ret;
		ret["result"] = "ok";
		ret["roles"] = mapToJson(roleList, [](const auto &r) { return r.toJson(); });
		ret["count"] = static_cast<int>(roleList.size());
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

void RoleCtrl::create(const HttpRequestPtr &req,
					  std::function<void(const HttpResponsePtr &)> &&callback)
{
	LOG_DEBUG << "Role create called";
	try
	{
		DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
		orm::Mapper<Roles> mapper(clientPtr);
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
		Roles newRole(*json);
		mapper.insert(newRole);
		Json::Value ret;
		ret["result"] = "ok";
		ret["data"] = newRole.toJson();
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

void RoleCtrl::update(const HttpRequestPtr &req,
					  std::function<void(const HttpResponsePtr &)> &&callback, std::string roleId)
{
	LOG_DEBUG << "Role update called";
	DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
	orm::Mapper<Roles> mapper(clientPtr);
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
		Roles role = mapper.findByPrimaryKey(roleId);
		role.updateByJson(*json);
		mapper.update(role);
		Json::Value ret;
		ret["result"] = "ok";
		ret["data"] = role.toJson();
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

void RoleCtrl::remove(const HttpRequestPtr &req,
					  std::function<void(const HttpResponsePtr &)> &&callback, std::string roleId)
{
	LOG_DEBUG << "Role remove called";
	DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
	orm::Mapper<Roles> mapper(clientPtr);
	try
	{
		Roles role = mapper.findByPrimaryKey(roleId);
		mapper.deleteByPrimaryKey(roleId);
		Json::Value ret;
		ret["result"] = "ok";
		ret["role"] = role.toJson();
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
