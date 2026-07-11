#include "UserProjectSettingCtrl.h"

#include <optional>

#include <drogon/HttpResponse.h>
#include <jsoncpp/json/json.h>

#include "models/UserProjectSettings.h"
#include "sql/create-user-project.h"
#include "sql/update-user-project.h"
#include "utils/mapToJson.h"



#include "plugins/RequestContext/RequestContext.h"

using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::teams_manager;

namespace
{
bool parseRatio(const std::string &ratioStr, long double &ratio)
{
	try
	{
		size_t pos = 0;
		ratio = std::stold(ratioStr, &pos);
		return pos == ratioStr.size();
	}
	catch (const std::exception &)
	{
		return false;
	}
}

bool validateTotalRatioForUser(
	const DbClientPtr &clientPtr,
	const std::string &userId,
	long double newRatio,
	const std::optional<std::string> &excludeId,
	std::string &errorMessage)
{
	orm::Mapper<UserProjectSettings> mapper(clientPtr);
	Criteria criteria = Criteria(UserProjectSettings::Cols::_user_id, CompareOperator::EQ, userId);
	std::vector<UserProjectSettings> settingsList = mapper.findBy(criteria);

	long double total = 0.0L;
	for (const auto &setting : settingsList)
	{
		if (excludeId && setting.getValueOfId() == *excludeId)
		{
			continue;
		}

		if (!setting.getExcecutorTimeRatio())
		{
			errorMessage = "Existing data has null 工時比例";
			return false;
		}

		long double value = 0.0L;
		if (!parseRatio(setting.getValueOfExcecutorTimeRatio(), value))
		{
			errorMessage = "Existing data has invalid 工時比例";
			return false;
		}
		total += value;
	}

	total += newRatio;
	if (total > 1.0L + 1e-12L)
	{
		errorMessage = "工時比例總計不可以超過100%";
		return false;
	}

	return true;
}
}  // namespace

void UserProjectSettingCtrl::get(
	const HttpRequestPtr &req,
	std::function<void(const HttpResponsePtr &)> &&callback,
	std::string id)
{
	(void)req;
	LOG_DEBUG << "UserProjectSettingCtrl get called";
	DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
	orm::Mapper<UserProjectSettings> mapper(clientPtr);

	try
	{
		UserProjectSettings item = mapper.findByPrimaryKey(id);
		Json::Value ret;
		ret["result"] = "ok";
		ret["data"] = item.toJson();
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



void UserProjectSettingCtrl::list(
	const HttpRequestPtr &req,
	std::function<void(const HttpResponsePtr &)> &&callback)
{
	(void)req;
	LOG_DEBUG << "UserProjectSettingCtrl list called";
	try
	{
		DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
		orm::Mapper<UserProjectSettings> mapper(clientPtr);
        std::string userId = RequestContext::getUserId();
        Criteria criteria = Criteria(UserProjectSettings::Cols::_user_id, CompareOperator::EQ, userId);
		std::vector<UserProjectSettings> settingsList = mapper.findBy(criteria);
		Json::Value ret;
		ret["result"] = "ok";
		ret["data"] = mapToJson(
			settingsList,
			[](const auto &item)
			{ return item.toJson(); });
		ret["count"] = static_cast<int>(settingsList.size());

		drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(ret);
		callback(resp);
	}
	catch (const DrogonDbException &e)
	{
		LOG_ERROR << "Database error: " << e.base().what();

		Json::Value error;
		error["result"] = "error";
		error["message"] = e.base().what();

		drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(error);
		resp->setStatusCode(k500InternalServerError);
		callback(resp);
	}
}



void UserProjectSettingCtrl::create(
	const HttpRequestPtr &req,
	std::function<void(const HttpResponsePtr &)> &&callback)
{
	LOG_DEBUG << "UserProjectSettingCtrl create called";
	DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
	orm::Mapper<UserProjectSettings> mapper(clientPtr);
	std::string userId = RequestContext::getUserId();
	try
	{
		const std::shared_ptr<Json::Value> json = req->getJsonObject();
		
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

		UserProjectSettings newItem(*json);
		newItem.setUserId(userId);
		if (!newItem.getProjectId())
		{
			Json::Value error;
			error["result"] = "error";
			error["message"] = "project_id is required";
			drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(error);
			resp->setStatusCode(k400BadRequest);
			callback(resp);
			return;
		}

		long double newRatio = 1.0L;
		if (newItem.getExcecutorTimeRatio())
		{
			if (!parseRatio(newItem.getValueOfExcecutorTimeRatio(), newRatio))
			{
				Json::Value error;
				error["result"] = "error";
				error["message"] = "無效的工時比例";
				drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(error);
				resp->setStatusCode(k400BadRequest);
				callback(resp);
				return;
			}
		}

		std::string ratioValidationError;
		if (!validateTotalRatioForUser(clientPtr, userId, newRatio, std::nullopt, ratioValidationError))
		{
			Json::Value error;
			error["result"] = "error";
			error["message"] = ratioValidationError;
			drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(error);
			resp->setStatusCode(k400BadRequest);
			callback(resp);
			return;
		}

		clientPtr->execSqlSync(
			createUserProjectSql,
			userId,
			newItem.getValueOfProjectId(),
			newItem.getValueOfExcecutorTimeRatio());

		Json::Value ret;
		ret["result"] = "ok";
		ret["data"] = newItem.toJson();

		drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(ret);
		callback(resp);
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

void UserProjectSettingCtrl::update(
	const HttpRequestPtr &req,
	std::function<void(const HttpResponsePtr &)> &&callback,
	std::string id)
{
	LOG_DEBUG << "UserProjectSettingCtrl update called";
	DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
	orm::Mapper<UserProjectSettings> mapper(clientPtr);
	std::string userId = RequestContext::getUserId();

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

		UserProjectSettings item = mapper.findByPrimaryKey(id);
		if (item.getValueOfUserId() != userId)
		{
			Json::Value error;
			error["result"] = "error";
			error["message"] = "Forbidden";
			drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(error);
			resp->setStatusCode(k403Forbidden);
			callback(resp);
			return;
		}

		item.updateByJson(*json);
		item.setUserId(userId);

		if (!item.getExcecutorTimeRatio())
		{
			Json::Value error;
			error["result"] = "error";
			error["message"] = "Invalid excecutor_time_ratio";
			drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(error);
			resp->setStatusCode(k400BadRequest);
			callback(resp);
			return;
		}

		long double newRatio = 0.0L;
		if (!parseRatio(item.getValueOfExcecutorTimeRatio(), newRatio))
		{
			Json::Value error;
			error["result"] = "error";
			error["message"] = "Invalid excecutor_time_ratio";
			drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(error);
			resp->setStatusCode(k400BadRequest);
			callback(resp);
			return;
		}

		std::string ratioValidationError;
		if (!validateTotalRatioForUser(clientPtr, userId, newRatio, id, ratioValidationError))
		{
			Json::Value error;
			error["result"] = "error";
			error["message"] = ratioValidationError;
			drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(error);
			resp->setStatusCode(k400BadRequest);
			callback(resp);
			return;
		}

		clientPtr->execSqlSync(
			updateUserProjectSql,
			id,
			userId,
			item.getValueOfProjectId(),
			item.getValueOfExcecutorTimeRatio());

		Json::Value ret;
		ret["result"] = "ok";
		ret["data"] = item.toJson();

		drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(ret);
		callback(resp);
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

void UserProjectSettingCtrl::remove(
	const HttpRequestPtr &req,
	std::function<void(const HttpResponsePtr &)> &&callback,
	std::string id)
{
	(void)req;
	LOG_DEBUG << "UserProjectSettingCtrl remove called";
	DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
	orm::Mapper<UserProjectSettings> mapper(clientPtr);

	try
	{
		UserProjectSettings item = mapper.findByPrimaryKey(id);
		mapper.deleteByPrimaryKey(id);

		Json::Value ret;
		ret["result"] = "ok";
		ret["data"] = item.toJson();

		drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(ret);
		callback(resp);
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
