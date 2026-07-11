#include "MilestoneCtrl.h"
#include <jsoncpp/json/json.h>
#include <drogon/HttpResponse.h>
#include "models/Milestones.h"
#include "models/MilestoneEstimationHistories.h"
#include "utils/mapToJson.h"
#include "utils/HttpErrorHandler.h"
#include "jwt/jwt.hpp"
#include "models/TMMapper.h"
#include "get-histories-and-tasks-sql.h"
#include "utils/TimeToDateString.h"
// #include <chrono>
#include "utils/BussinessDays.h"
#include "constants/Coefficient.h"
using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::teams_manager;



void Milestone::complete(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback, 
    std::string milestoneId
) {
    DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");

    if (milestoneId.empty())
    {
        callback(makeErrorResponse(
            k400BadRequest,
            "milestone_id is empty"));
        return;
    }

    clientPtr->execSqlAsync(
        R"(
            UPDATE milestones 
            SET completed = true, 
                completion_date = CURRENT_DATE
            WHERE id = $1
              AND NOT EXISTS (
              SELECT 1
                FROM tasks t
                WHERE t.milestone_id = $1
                    AND t.completed = false
            );
        )",
        [callback](const Result &r)
        {
            if (r.affectedRows() == 0)
            {
                callback(makeErrorResponse(
                    k400BadRequest,
                    "有工作未完成"));
                return;
            }
            Json::Value ret;
            ret["result"] = "ok";
            callback(HttpResponse::newHttpJsonResponse(ret));
        },
        [callback](const DrogonDbException &e)
        {
            LOG_ERROR << "Database error: " << e.base().what();
            callback(makeErrorResponse(
                k500InternalServerError,
                "Database error: " + std::string(e.base().what())));
        },
        milestoneId);
}