#include "TaskCtrl.h"
#include "models/Tasks.h"
#include "plugins/RequestContext/RequestContext.h"
#include "utils/mapToJson.h"
#include "utils/week_time.h"
#include "sql/sql.h"
using namespace drogon::orm;



void TaskCtrl::create(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback)
{
    try
    {
        LOG_DEBUG << "Task create called";

        drogon::orm::DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");

        // ===== Parse JSON body =====
        const std::shared_ptr<Json::Value> json = req->getJsonObject();
        if (!json)
        {
            drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(
                Json::Value("invalid json body"));
            resp->setStatusCode(k400BadRequest);
            callback(resp);
            return;
        }

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
        json->removeMember("created_at");
        json->removeMember("completion_date");
        if (!json->isMember("this_week"))
        {
            (*json)["this_week"] = true;
        }

        
        std::string creatorId = RequestContext::getUserId();
        std::string tenantId = RequestContext::getTenantId();
        std::string projectId = (*json)["project_id"].asString();
        std::string milestoneId = (*json)["milestone_id"].asString();
        std::string teamId = RequestContext::getTeamId();

        std::string taskTypeId = "";

        if ((*json).isMember("task_type_id") && !(*json)["task_type_id"].isNull())
        {
            taskTypeId = (*json)["task_type_id"].asString();
        }
        else
        {
            taskTypeId = ""; // ❗先暫代 NULL
        }

        // 處理 this_week 自動日期
        // bool thisWeek = (*json)["this_week"].asBool();
        // if (json->isMember("this_week"))
        // {
        //     if (thisWeek)
        //     {
        //         (*json)["scheduled_start_date"] = WeekTimeUtils::getThisMonday();
        //         (*json)["scheduled_end_date"] = WeekTimeUtils::getThisFriday();
        //     }
        //     else
        //     {
        //         (*json)["scheduled_start_date"] = WeekTimeUtils::getNextMonday();
        //         (*json)["scheduled_end_date"] = WeekTimeUtils::getNextFriday();
        //     }
        // }

        // LOG_DEBUG << "===thisWeek===: " << thisWeek;

        std::string taskTypeName = (*json)["task_type_name"].asString();

        std::string name = (*json)["name"].asString();
        std::string description = (*json)["description"].asString();

        std::string scheduledStartDate = (*json)["scheduled_start_date"].asString();
        std::string scheduledEndDate = (*json)["scheduled_end_date"].asString();

        std::string statusId = (*json)["status_id"].asString();
        std::string statusName = (*json)["status_name"].asString();

        int estimatedWorkdays = (*json)["estimated_workdays"].asInt();
        int priority = (*json)["priority"].asInt();

        // ===== arrays =====
        std::vector<std::string> executorIds;
        std::vector<std::string> mentorIds;
        std::vector<Json::Value> dependencies;

        for (auto &v : (*json)["executor_ids"])
            executorIds.push_back(v.asString());

        for (auto &v : (*json)["mentor_ids"])
            mentorIds.push_back(v.asString());

        for (auto &v : (*json)["dependencies"])
            dependencies.push_back(v);

        // helper: convert vector -> PostgreSQL uuid[]
        auto toPgArray = [](const std::vector<std::string> &vec)
        {
            std::string result = "{";
            for (size_t i = 0; i < vec.size(); i++)
            {
                result += vec[i];
                if (i != vec.size() - 1)
                    result += ",";
            }
            result += "}";
            return result;
        };

        // helper: convert Json::Value array -> JSON array string for JSONB
        auto toPgJsonArray = [](const std::vector<Json::Value> &vec)
        {
            Json::Value arrayJson(Json::arrayValue);
            for (const auto &v : vec)
            {
                arrayJson.append(v);
            }
            Json::StreamWriterBuilder writerBuilder;
            return Json::writeString(writerBuilder, arrayJson);
        };

        std::string executorArray = toPgArray(executorIds);
        std::string mentorArray = toPgArray(mentorIds);
        std::string dependencyArray = toPgJsonArray(dependencies);

        LOG_DEBUG << "Creating task with data: " << executorArray << ", " << mentorArray;

        drogon::orm::Result result = clientPtr->execSqlSync(
            createTasksSql,
            projectId,
            milestoneId,
            teamId,

            creatorId,
            // creatorName,
            tenantId,
            // tenantName,

            taskTypeId,
            taskTypeName,

            name,
            description,

            scheduledStartDate,
            scheduledEndDate,

            statusId,
            statusName,

            estimatedWorkdays,
            
            executorArray,
            mentorArray,
            dependencyArray);
            // dependencyArray);

        Json::Value ret;
        Json::Value data;
        data["id"] = result[0]["id"].as<std::string>();
        data["name"] = result[0]["name"].as<std::string>();
        ret["result"] = "ok";
        ret["data"] = data;
        drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(k200OK);
        callback(resp);
    }
    catch (const DrogonDbException &e)
    {
        LOG_ERROR << "DB error: " << e.base().what();

        Json::Value err;
        err["error"] = e.base().what();

        auto resp = HttpResponse::newHttpJsonResponse(err);
        resp->setStatusCode(k500InternalServerError);
        callback(resp);
    }
}