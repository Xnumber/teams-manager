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

// 工作大項估計歷史
// 現在日期
// 現在完成工作日統計
// 現在未完成工作列表
// 剩餘工作日統計
// 預計接受日
// 樂觀預計接受日
// 悲觀預計接受日

void Milestone::acceptanceTimeConvergence(const HttpRequestPtr &req,
                                          std::function<void(const HttpResponsePtr &)> &&callback)
{
    DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
    std::string milestoneId = req->getParameter("milestone_id");

    if (milestoneId.empty())
    {
        callback(makeErrorResponse(
            k400BadRequest,
            "milestone_id is empty"));
        return;
    }

    clientPtr->execSqlAsync(
        getMilestoneEstimationHistoriesSql(),
        [callback](const Result &r)
        {
            if (r.size() == 0)
            {
                auto now = std::chrono::system_clock::now();
                auto now_time_t = std::chrono::system_clock::to_time_t(now);
                std::string current_date_str = timeToDateString(now_time_t);

                Json::Value ret;
                ret["result"] = "ok";
                // ret["data"]["histories"] = Json::Value(Json::arrayValue);
                // ret["data"]["tasks"] = Json::Value(Json::arrayValue);
                ret["data"]["remaining_working_days"] = 0;
                ret["data"]["current_date"] = current_date_str;
                ret["data"]["predicted_acceptance_date"] = current_date_str;
                ret["data"]["optimistic_acceptance_date"] = current_date_str;
                ret["data"]["pessimistic_acceptance_date"] = current_date_str;
                callback(HttpResponse::newHttpJsonResponse(ret));
                return;
            }

            Json::Value data;
            data["histories"] = Json::Value(Json::arrayValue);
            // data["tasks"] = Json::Value(Json::arrayValue);
            for (const auto &row : r)
            {
                Json::Value history;
                history["id"] = row["id"].as<std::string>();
                history["milestone_id"] = row["milestone_id"].as<std::string>();
                history["estimation_date"] = row["estimation_date"].as<std::string>();
                history["complete_date"] = row["complete_date"].as<std::string>();
                history["optimistic_estimated_complete_date"] = row["optimistic_estimated_complete_date"].as<std::string>();
                history["pessimistic_estimated_complete_date"] = row["pessimistic_estimated_complete_date"].as<std::string>();
                history["created_at"] = row["created_at"].as<std::string>();
                history["estimated_remaining_workdays"] = row["estimated_remaining_workdays"].as<int>();
                history["left_tasks"] = row["left_tasks"].as<int>();
                data["histories"].append(history);
            }

            // 計算剩餘工作天數（未完成工作的 estimated_workdays 總和）
            int remaining_working_days = 0;
            if (r.size() > 0 && !r[0]["estimated_remaining_workdays"].isNull())
            {
                remaining_working_days = r[0]["estimated_remaining_workdays"].as<int>();
            }

            // 取得今天日期
            std::chrono::_V2::system_clock::time_point now = std::chrono::system_clock::now();
            std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
            double optimistic_remaining_working_days = static_cast<double>(remaining_working_days * OPTIMISTIC_COEFFICIENT); 
            double pessimistic_remaining_working_days = static_cast<double>(remaining_working_days * PESSIMISTIC_COEFFICIENT); 
            // 計算預計接受時間（以今天為基準， 預計、樂觀、悲觀）
            long predicted_acceptance_time = addBusinessDays(now_time_t, static_cast<long>(remaining_working_days));
            long optimistic_acceptance_time = addBusinessDays(now_time_t, static_cast<long>(std::lround(optimistic_remaining_working_days)));   // 樂觀估計
            long pessimistic_acceptance_time = addBusinessDays(now_time_t, static_cast<long>(std::lround(pessimistic_remaining_working_days))); // 悲觀估計

            // 轉換為日期字串
            std::string current_date_str = timeToDateString(now_time_t);
            std::string predicted_date_str = timeToDateString(predicted_acceptance_time);
            std::string optimistic_date_str = timeToDateString(optimistic_acceptance_time);
            std::string pessimistic_date_str = timeToDateString(pessimistic_acceptance_time);

            Json::Value ret;
            ret["result"] = "ok";
            ret["data"] = data;
            ret["data"]["remaining_working_days"] = remaining_working_days;
            ret["data"]["current_date"] = current_date_str;
            ret["data"]["predicted_acceptance_date"] = predicted_date_str;
            ret["data"]["optimistic_acceptance_date"] = optimistic_date_str;
            ret["data"]["pessimistic_acceptance_date"] = pessimistic_date_str;
            ret["data"]["optimistic_remaining_working_days"] = optimistic_remaining_working_days;
            ret["data"]["pessimistic_remaining_working_days"] = pessimistic_remaining_working_days;
            callback(HttpResponse::newHttpJsonResponse(ret));
        },
        [callback](const DrogonDbException &e)
        {
            LOG_ERROR << "Database error: " << e.base().what();
            callback(makeErrorResponse(k500InternalServerError,
                                       std::string("Database error: ") + e.base().what()));
        },
        milestoneId);
}