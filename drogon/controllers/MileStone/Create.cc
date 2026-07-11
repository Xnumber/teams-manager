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

void Milestone::create(const HttpRequestPtr &req,
					   std::function<void(const HttpResponsePtr &)> &&callback)
{
	jwt::jwt_payload jwtPayload = req->getAttributes()->get<jwt::jwt_payload>("jwt_payload");
	std::string tenant_id = jwtPayload.get_claim_value<std::string>("tenant_id");
	LOG_DEBUG << "Milestone create called";
	try
	{
		DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
		TMMapper<Milestones> mapper(clientPtr);
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

		std::string project_id = (*json)["project_id"].asString();
		std::string name = (*json)["name"].asString();
		std::string description = (*json).isMember("description") ? (*json)["description"].asString() : "";
		std::string project_name = (*json).isMember("project_name") ? (*json)["project_name"].asString() : "";
		std::string team_id = (*json).isMember("team_id") ? (*json)["team_id"].asString() : "";
		std::string team_name = (*json).isMember("team_name") ? (*json)["team_name"].asString() : "";
		std::string tenant_name = (*json).isMember("tenant_name") ? (*json)["tenant_name"].asString() : "";
		std::string start_date = (*json).isMember("start_date") ? (*json)["start_date"].asString() : "";
		std::string end_date = (*json).isMember("end_date") ? (*json)["end_date"].asString() : "";
		std::string created_at = (*json).isMember("created_at") ? (*json)["created_at"].asString() : "";
		std::string creator_id = (*json).isMember("creator_id") ? (*json)["creator_id"].asString() : "";
		std::string creator_name = (*json).isMember("creator_name") ? (*json)["creator_name"].asString() : "";
		std::string concurrency_stamp = drogon::utils::getUuid();
		std::string remark = (*json).isMember("remark") ? (*json)["remark"].asString() : "";
		std::string estimated_remaining_workdays = (*json).isMember("estimated_remaining_workdays")
													 ? std::to_string((*json)["estimated_remaining_workdays"].asInt())
													 : "";
		
		bool is_for_demo = (*json).isMember("is_for_demo") ? (*json)["is_for_demo"].asBool() : false;

		Json::Value dependencies = (*json).isMember("dependencies") ? (*json)["dependencies"] : Json::Value(Json::arrayValue);

        std::cout << "Received JSON for creating milestone: " << (*json).toStyledString() << std::endl;
		std::cout << concurrency_stamp << std::endl;
		clientPtr->execSqlAsync(
			R"SQL(
				WITH next_priority AS (
					SELECT
						COALESCE(MAX(priority), 0) + 1 AS priority
					FROM milestones
					WHERE project_id = $2::uuid
				),
				new_milestone AS (
					INSERT INTO milestones (
						name,
						description,
						project_id,
						project_name,
						team_id,
						team_name,
						tenant_id,
						tenant_name,
						start_date,
						end_date,
						created_at,
						creator_id,
						creator_name,
						concurrency_stamp,
						remark,
						priority,
						estimated_remaining_workdays,
						is_for_demo
					)
					SELECT
						$3,
						NULLIF($4, ''),
						$2::uuid,
						NULLIF($5, ''),
						NULLIF($6, '')::uuid,
						NULLIF($7, ''),
						$1::uuid,
						NULLIF($8, ''),
						NULLIF($9, '')::date,
						NULLIF($10, '')::date,
						COALESCE(NULLIF($11, '')::timestamp, NOW()),
						NULLIF($12, '')::uuid,
						NULLIF($13, ''),
						$14,
						NULLIF($15, ''),
						next_priority.priority,
						COALESCE(NULLIF($16, '')::integer, 0),
						$17::boolean
					FROM next_priority
					RETURNING *
				),
				parsed_dependencies AS (
					SELECT
						nm.id as milestone_id,
						(dep->>'predecessorType')::work_item_type as pred_type,
						(dep->>'predecessorId')::uuid as pred_id,
						COALESCE((dep->>'dependencyType'), 'FS')::work_item_dependency_type as dep_type
					FROM new_milestone nm,
						 jsonb_array_elements(COALESCE($18::jsonb, '[]'::jsonb)) AS dep
				),
				insert_dependencies AS (
					INSERT INTO work_item_dependencies (
						predecessor_type,
						predecessor_id,
						successor_type,
						successor_id,
						dependency_type
					)
					SELECT
						pred_type,
						pred_id,
						'MILESTONE'::work_item_type,
						milestone_id,
						dep_type
					FROM parsed_dependencies
				)
				SELECT * FROM new_milestone;
			)SQL",
			[callback](const Result &r)
			{
				if (r.empty())
				{
					Json::Value err;
					err["result"] = "error";
					err["message"] = "Create milestone failed";
					auto resp = HttpResponse::newHttpJsonResponse(err);
					resp->setStatusCode(k500InternalServerError);
					callback(resp);
					return;
				}

				Milestones newMilestone(r[0], -1);

				Json::Value ret;
				ret["result"] = "ok";
				ret["data"] = newMilestone.toJson();
				auto resp = HttpResponse::newHttpJsonResponse(ret);
				callback(resp);
			},
			[callback](const DrogonDbException &e)
			{
				LOG_ERROR << "Milestone create error: " << e.base().what();
				Json::Value err;
				err["result"] = "error";
				err["message"] = std::string("Create failed: ") + e.base().what();
				auto resp = HttpResponse::newHttpJsonResponse(err);
				resp->setStatusCode(k500InternalServerError);
				callback(resp);
			},
			tenant_id,
			project_id,
			name,
			description,
			project_name,
			team_id,
			team_name,
			tenant_name,
			start_date,
			end_date,
			created_at,
			creator_id,
			creator_name,
			concurrency_stamp,
			remark,
			estimated_remaining_workdays,
			is_for_demo,
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