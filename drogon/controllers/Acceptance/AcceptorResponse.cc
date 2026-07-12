#include "AcceptanceCtrl.h"
#include <jsoncpp/json/json.h>
#include <drogon/HttpResponse.h>
#include "jwt/jwt.hpp"
#include "sql/sql.h"
#include "plugins/RequestContext/RequestContext.h"
#include "models/PlansProjects.h"
#include <optional>
#include "utils/optStr.h"
using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::teams_manager;


void AcceptanceCtrl::acceptorResponse(const HttpRequestPtr &req,
								   std::function<void(const HttpResponsePtr &)> &&callback)
{
    LOG_DEBUG << "Acceptance request acceptor response called";
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

		std::string task_id = (*json)["task_id"].asString();
		std::string acceptor_id = (*json)["acceptor_id"].asString();
		bool accepted = (*json)["accepted"].asBool();
		std::optional<std::string> reject_type_id = optStr(json, "reject_type_id");
		std::optional<std::string> reject_reason = optStr(json, "reject_reason");
		std::optional<std::string> acceptor_comment = optStr(json, "acceptor_comment");
		std::optional<std::string> reviewed_at = optStr(json, "reviewed_at");

		clientPtr->execSqlAsync(
			acceptorResponseAcceptanceRequestSql,
			[callback](const Result &r)
			{
				if (r.empty())
				{
					Json::Value err;
					err["result"] = "error";
					err["message"] = "Acceptor Response failed";
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
					if (field.isNull())
						data[colName] = Json::Value::null;
					else
						data[colName] = field.as<std::string>();
				}
				ret["data"] = data;

				drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(ret);
				callback(resp);
			},
			[callback](const DrogonDbException &e)
			{
				LOG_ERROR << "Acceptance Acceptor Response Error: " << e.base().what();
				Json::Value err;
				err["result"] = "error";
				err["message"] = std::string("Acceptor Response failed: ") + e.base().what();
				auto resp = HttpResponse::newHttpJsonResponse(err);
				resp->setStatusCode(k500InternalServerError);
				callback(resp);
			},
			task_id,
			acceptor_id,
			accepted,
			reject_type_id,
			reject_reason,
			acceptor_comment,
			reviewed_at
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