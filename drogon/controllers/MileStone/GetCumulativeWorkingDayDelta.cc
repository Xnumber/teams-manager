#include "MilestoneCtrl.h"
#include "sql/get-milestone-complete-date.h"
#include <jsoncpp/json/json.h>
#include <drogon/HttpResponse.h>
#include <ctime>
#include <sstream>
#include <cmath>
#include <iomanip>
#include "models/Milestones.h"
#include "utils/mapToJson.h"
#include "jwt/jwt.hpp"
#include "models/TMMapper.h"
using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::teams_manager;

namespace
{
/**
 * 將日期字串解析為 time_t（僅使用日期精度）。
 *
 * - 輸入至少需包含前 10 個字元，格式為 YYYY-MM-DD。
 * - 時間固定為本地時區 12:00:00，以降低夏令時間切換造成的邊界誤差。
 * - 當輸入無效或解析失敗時，回傳 -1。
 */
time_t parseDateOnlyToTimeT(const std::string &dateTimeText)
{
    if (dateTimeText.size() < 10)
    {
        return static_cast<time_t>(-1);
    }

    std::tm tm = {};
    std::istringstream ss(dateTimeText.substr(0, 10));
    ss >> std::get_time(&tm, "%Y-%m-%d");
    if (ss.fail())
    {
        return static_cast<time_t>(-1);
    }

    // 使用中午時間避免夏令時間切換造成邊界誤差。
    tm.tm_hour = 12;
    tm.tm_min = 0;
    tm.tm_sec = 0;
    tm.tm_isdst = -1;
    return std::mktime(&tm);
}
} // namespace

/**
 * 取得工作大項的累積工作日差異（Cumulative Working Day Delta）
 * 從milestone_estimation_histories中取得對應milestone_id的歷史估算紀錄
 * 利用每筆紀錄的complete_date與上一筆紀錄的complete_date計算工作日差異，並累積起來
 * sql回傳已經經過排序，所以可以直接在程式中計算差異
 * 回傳一個包含日期(milestone_estimation_histories的estimation_date)與累積工作日差異(cumulative_working_day_delta)的列表，供前端繪製圖表使用
 * @param req HTTP請求對象，包含milestone_id參數
 * @param callback 用於回傳HTTP響應的回調函數
 */
void Milestone::getCumulativeWorkingDayDelta(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback)
{
	LOG_DEBUG << "Milestone getCumulativeWorkingDayDelta called";
	DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
    std::string milestoneId = req->getParameter("milestone_id");

    if (milestoneId.empty())
    {
        Json::Value error;
        error["result"] = "error";
        error["message"] = "milestone_id is empty";
        drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(k400BadRequest);
        callback(resp);
        return;
    }

	try
	{
        clientPtr->execSqlAsync(
            getMilestoneCompleteDateSql,
            [callback, milestoneId](const drogon::orm::Result &r) {
                Json::Value ret;
                ret["result"] = "ok";
                ret["data"] = Json::arrayValue;

                time_t previousCompleteDate = static_cast<time_t>(-1);
                int cumulativeDeltaDays = 0;
                for (const auto &row : r)
                {
                    Json::Value item;
                    const std::string estimationDate = row["estimation_date"].as<std::string>();
                    const std::string completeDate = row["complete_date"].as<std::string>();

                    item["estimation_date"] = estimationDate;

                    const time_t currentCompleteDate = parseDateOnlyToTimeT(completeDate);
                    int deltaDays = 0;
                    if (previousCompleteDate != static_cast<time_t>(-1) &&
                        currentCompleteDate != static_cast<time_t>(-1))
                    {
                        const double secondsDiff = std::difftime(currentCompleteDate, previousCompleteDate);
                        deltaDays = static_cast<int>(std::llround(secondsDiff / 86400.0));
                    }

                    cumulativeDeltaDays += deltaDays;
                    item["delta"] = - cumulativeDeltaDays;
                    ret["data"].append(item);

                    if (currentCompleteDate != static_cast<time_t>(-1))
                    {
                        previousCompleteDate = currentCompleteDate;
                    }
                }

                drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(ret);
                callback(resp);
            },
            [callback](const DrogonDbException &e) {
                LOG_ERROR << "Database error: " << e.base().what();
                Json::Value error;
                error["result"] = "error";
                error["message"] = e.base().what();
                drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(error);
                resp->setStatusCode(k500InternalServerError);
                callback(resp);
            },
            milestoneId
        );
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