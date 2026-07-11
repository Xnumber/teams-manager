#include "ProjectCtrl.h"
#include "sql/get-project-complete-date.h"
#include <jsoncpp/json/json.h>
#include <drogon/HttpResponse.h>
#include <ctime>
#include <sstream>
#include <cmath>
#include <iomanip>
#include <regex>
#include "models/Milestones.h"
#include "utils/mapToJson.h"
#include "jwt/jwt.hpp"
#include "models/TMMapper.h"
#include "sql/get-participants-info.h"
using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::teams_manager;

/**
 * 取得專案的參與者訊息（Participants Info）
 * @param req HTTP請求對象，包含project_id參數
 * @param callback 用於回傳HTTP響應的回調函數
 */
void Project::getParticipantsInfo(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback,
    std::string projectId)
{
    LOG_DEBUG << "Project getParticipants called";
    DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");

    if (projectId.empty())
    {
        Json::Value error;
        error["result"] = "error";
        error["message"] = "project_id is empty";
        drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(k400BadRequest);
        callback(resp);
        return;
    }

    static const std::regex uuidPattern(
        "^[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[1-5][0-9a-fA-F]{3}-[89abAB][0-9a-fA-F]{3}-[0-9a-fA-F]{12}$");
    if (!std::regex_match(projectId, uuidPattern))
    {
        Json::Value error;
        error["result"] = "error";
        error["message"] = "project_id is not a valid UUID";
        drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(k400BadRequest);
        callback(resp);
        return;
    }

    try
    {
        clientPtr->execSqlAsync(
            getParticipantsInfoSql,
            [callback](const drogon::orm::Result &r)
            {
                Json::Value ret;
                ret["result"] = "ok";

                Json::Value data(Json::objectValue);
                data["count"] = 0;
                data["avg_excecutor_time_ratio"] = 0.0;
                data["sum_estimated_workdays"] = 0.0;

                // 聚合查詢理論上會有一筆，這裡保險處理
                if (!r.empty())
                {
                    const auto &row = r[0];
                    data["count"] = row["count"].as<int>();
                    data["avg_excecutor_time_ratio"] = row["avg_excecutor_time_ratio"].isNull()
                                      ? 0.0
                                      : row["avg_excecutor_time_ratio"].as<double>();
                    data["sum_estimated_workdays"] = row["sum_estimated_workdays"].isNull()
                                      ? 0.0
                                      : row["sum_estimated_workdays"].as<double>();
                }

                ret["data"] = data;

                drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(ret);
                callback(resp);
            },
            [callback](const DrogonDbException &e)
            {
                LOG_ERROR << "Database error: " << e.base().what();
                Json::Value error;
                error["result"] = "error";
                error["message"] = e.base().what();
                drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(error);
                resp->setStatusCode(k500InternalServerError);
                callback(resp);
            },
            projectId);
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