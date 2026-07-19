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

/**
 * 新增請求確認。
 */
void AcceptanceCtrl::createRequest(const HttpRequestPtr &req,
								   std::function<void(const HttpResponsePtr &)> &&callback)
{
    LOG_DEBUG << "Acceptance request create called";
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
		std::string user_id = RequestContext::getUserId();
		std::string task_id = (*json)["task_id"].asString();

		if (user_id.empty())
		{
			Json::Value error;
			error["result"] = "error";
			error["message"] = "Unauthorized: missing requester_id from request context";
			auto resp = HttpResponse::newHttpJsonResponse(error);
			resp->setStatusCode(k401Unauthorized);
			callback(resp);
			return;
		}

		if (task_id.empty())
		{
			Json::Value error;
			error["result"] = "error";
			error["message"] = "task_id is required";
			auto resp = HttpResponse::newHttpJsonResponse(error);
			resp->setStatusCode(k400BadRequest);
			callback(resp);
			return;
		}

		// serialize acceptors array to JSON string for SQL jsonb parameter
		std::string acceptorsJson;
		if (json->isMember("acceptors") && (*json)["acceptors"].isArray())
		{
			Json::StreamWriterBuilder writerBuilder;
			acceptorsJson = Json::writeString(writerBuilder, (*json)["acceptors"]);
		}
		else
		{
			acceptorsJson = "[]";
		}

		if (acceptorsJson == "[]")
		{
			Json::Value error;
			error["result"] = "error";
			error["message"] = "acceptors is required";
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

				const Row &row = r[0];
				Json::Value data(Json::objectValue);
				for (size_t i = 0; i < row.size(); ++i)
				{
					const auto &field = row[i];
					const std::string colName = r.columnName(i);
					if (colName == "acceptors")
					{
						Json::Value parsed;
						Json::CharReaderBuilder readerBuilder;
						std::string errs;
						std::string s = field.as<std::string>();
						std::istringstream iss(s);
						if (!Json::parseFromStream(readerBuilder, iss, &parsed, &errs))
						{
							data[colName] = Json::Value::null;
						}
						else
						{
							data[colName] = parsed;
						}
					}
					else if (field.isNull())
					{
						data[colName] = Json::Value::null;
					}
					else
					{
						data[colName] = field.as<std::string>();
					}
				}
				ret["data"] = data;

				drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(ret);
				callback(resp);
			},
			[callback](const DrogonDbException &e)
			{
				const std::string dbMessage = e.base().what();
				LOG_ERROR << "Acceptance Request Error: " << dbMessage;
				Json::Value err;
				err["result"] = "error";
				err["message"] = std::string("Create failed: ") + dbMessage;
				auto resp = HttpResponse::newHttpJsonResponse(err);

				const bool isDuplicateRequester =
					dbMessage.find("unique_task_requester") != std::string::npos ||
					dbMessage.find("duplicate key value") != std::string::npos;

				resp->setStatusCode(isDuplicateRequester ? k409Conflict : k500InternalServerError);
				callback(resp);
			},
			user_id,
			task_id,
			acceptorsJson
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