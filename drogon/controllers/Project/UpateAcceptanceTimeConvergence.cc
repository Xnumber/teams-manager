#include "ProjectCtrl.h"
#include <jsoncpp/json/json.h>
#include <drogon/HttpResponse.h>
#include "models/Projects.h"
#include "models/ProjectEstimationHistories.h"
#include "utils/mapToJson.h"
#include "utils/HttpErrorHandler.h"
#include "jwt/jwt.hpp"
#include "models/TMMapper.h"
#include "utils/TimeToDateString.h"
#include "utils/BussinessDays.h"
#include <chrono>
#include <map>
#include "constants/Coefficient.h"
#include "get-histories-and-milestones-sql.h"
#include "sql/update-project-estimation-by-latest-milestone.h"
using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::teams_manager;

// 工作大項估計歷史
// 現在日期
// 現在完成工作日統計
// 現在未完成工作列表
// 剩餘工作日統計
// 預計接受日
// 樂觀預計接受日
// 悲觀預計接受日








void Project::updateAcceptanceTimeConvergence(const HttpRequestPtr &req,
                                              std::function<void(const HttpResponsePtr &)> &&callback)
{
    DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
    std::string projectId = req->getParameter("project_id");

    if (projectId.empty())
    {
        callback(makeErrorResponse(
            k400BadRequest,
            "project_id is empty"));
        return;
    }
    
    clientPtr->execSqlAsync(
        updateProjectEstimationDatesByLatestMilestone(),
        [callback, clientPtr, projectId](const Result &r)
        {
            Json::Value ret;
            ret["result"] = "ok";
            ret["data"] = Json::Value(Json::arrayValue);

            for (const auto &row : r)
            {
                Json::Value item;
                item["project_id"] = row["project_id"].as<std::string>();
                if (!row["estimated_end_date"].isNull())
                {
                    item["estimated_end_date"] = row["estimated_end_date"].as<std::string>();
                }
                if (!row["optimistic_estimated_complete_date"].isNull())
                {
                    item["optimistic_estimated_complete_date"] = row["optimistic_estimated_complete_date"].as<std::string>();
                }
                if (!row["pessimistic_estimated_complete_date"].isNull())
                {
                    item["pessimistic_estimated_complete_date"] = row["pessimistic_estimated_complete_date"].as<std::string>();
                }
                ret["data"].append(item);
            }

            callback(HttpResponse::newHttpJsonResponse(ret));
        },
        [callback](const DrogonDbException &e)
        {
            LOG_ERROR << "Database error: " << e.base().what();
            callback(makeErrorResponse(k500InternalServerError,
                                       std::string("Database error: ") + e.base().what()));
        },
        projectId);
}