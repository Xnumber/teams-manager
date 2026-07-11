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
#include "sql/update-acceptance-time-convergence-sql.h"
#include "utils/TimeToDateString.h"
#include "utils/BussinessDays.h"
#include <ctime>
// #include <chrono>
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

void Milestone::updateAcceptanceTimeConvergence(const HttpRequestPtr &req,
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
        getMilestoneTasksSql(),
        [callback, clientPtr, milestoneId](const Result &r)
        {
            // std::cout << "Milestone ID: " << milestoneId << std::endl;



            Json::Value data;

            int remaining_working_days = 0;

            if (r.size() > 0 && !r[0]["total_estimated_workdays"].isNull())
            {
                remaining_working_days = r[0]["total_estimated_workdays"].as<int>();
            }
   
            LOG_DEBUG << "Remaining working days: " << remaining_working_days;

            // 取得今天日期
            std::chrono::_V2::system_clock::time_point now = std::chrono::system_clock::now();
            std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
            // 如果有依賴的工作大項，則以依賴工作大項的預計結束日期作為現在的日期，因為這代表了當前工作大項最早可能開始的時間點
            if (r.size() > 0 && !r[0]["dependency_estimated_end_date"].isNull())
            {
                std::tm dependency_tm = {};
                const std::string dependency_end_date = r[0]["dependency_estimated_end_date"].as<std::string>();
                if (strptime(dependency_end_date.c_str(), "%Y-%m-%d", &dependency_tm) != nullptr)
                {
                    now_time_t = std::mktime(&dependency_tm);
                }
            }

            double optimistic_remaining_working_days = static_cast<double>(remaining_working_days * OPTIMISTIC_COEFFICIENT);
            double pessimistic_remaining_working_days = static_cast<double>(remaining_working_days * PESSIMISTIC_COEFFICIENT);
            long predicted_acceptance_time = addBusinessDays(now_time_t, static_cast<long>(remaining_working_days));
            long optimistic_acceptance_time = addBusinessDays(now_time_t, static_cast<long>(std::lround(optimistic_remaining_working_days)));   // 樂觀估計
            long pessimistic_acceptance_time = addBusinessDays(now_time_t, static_cast<long>(std::lround(pessimistic_remaining_working_days))); // 悲觀估計

            // 轉換為日期字串
            std::string current_date_str = timeToDateString(now_time_t);
            std::string predicted_date_str = timeToDateString(predicted_acceptance_time);
            std::string optimistic_date_str = timeToDateString(optimistic_acceptance_time);
            std::string pessimistic_date_str = timeToDateString(pessimistic_acceptance_time);

            std::cout << "Predicted acceptance date: " << predicted_date_str << std::endl;
            std::cout << "Optimistic acceptance date: " << optimistic_date_str << std::endl;
            std::cout << "Pessimistic acceptance date: " << pessimistic_date_str << std::endl;


            // 保存到資料庫
            // 使用 upsert 的方式，如果當天已經有紀錄就更新，沒有就插入
            // 這裡的 upsert 是先刪除當天的紀錄（如果有的話），然後插入新的紀錄。這樣可以確保每天只有一筆紀錄，並且不需要考慮更新時的條件判斷。
            // 注意：這種方式在高併發的情況下可能會有 race condition，如果有多個請求同時更新同一個工作大項，可能會導致刪除和插入的操作交錯，從而產生不一致的數據。根據實際情況，可能需要加鎖或者使用資料庫的 upsert 功能（如 PostgreSQL 的 ON CONFLICT）來避免這個問題。
            
            clientPtr->execSqlAsync(
                getUpdateAcceptanceTimeConvergenceSql(),
                [callback, data, remaining_working_days, current_date_str, predicted_date_str,
                 optimistic_date_str, pessimistic_date_str, optimistic_remaining_working_days,
                 pessimistic_remaining_working_days, milestoneId](const Result &saveResult)
                {
                    // std::cout << "Upsert milestoneId: " << milestoneId << std::endl;
                    // std::cout << "Upsert predicted_date_str: " << predicted_date_str << std::endl;
                    // std::cout << "Upsert optimistic_date_str: " << optimistic_date_str << std::endl;
                    // std::cout << "Upsert pessimistic_date_str: " << pessimistic_date_str << std::endl;
                    
                    Json::Value ret;
                    ret["result"] = "ok";
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
                    LOG_ERROR << "Failed to save estimation history: " << e.base().what();
                    callback(makeErrorResponse(k500InternalServerError,
                                               std::string("Failed to save estimation: ") + e.base().what()));
                },
                milestoneId,
                predicted_date_str,
                optimistic_date_str,
                pessimistic_date_str);
        },
        [callback](const DrogonDbException &e)
        {
            LOG_ERROR << "Database error: " << e.base().what();
            callback(makeErrorResponse(k500InternalServerError,
                                       std::string("Database error: ") + e.base().what()));
        },
        milestoneId);
}