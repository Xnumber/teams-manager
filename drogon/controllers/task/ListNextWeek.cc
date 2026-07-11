#include "TaskCtrl.h"
#include "models/Tasks.h"
#include "plugins/RequestContext/RequestContext.h"
#include "utils/mapToJson.h"
#include "utils/week_time.h"
#include "sql/sql.h"
using namespace drogon::orm;



void TaskCtrl::listNextWeek(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback)
{
    try
    {
        DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");

        // std::string userId = RequestContext::getUserId();
        std::string executorId = req->getParameter("executor_id");
        std::string description = req->getParameter("description");
        std::string name = req->getParameter("name");
        std::string project_name = req->getParameter("project_name");
        std::optional<std::string> milestoneParam;
        std::string milestoneId = req->getParameter("milestone_id");
        std::optional<std::string> statusParam;
        std::string status_id = req->getParameter("status_id");

        if (milestoneId.empty())
        {
            milestoneParam = std::nullopt;
        }
        else
        {
            milestoneParam = milestoneId;
        }

        if (status_id.empty())
        {
            statusParam = std::nullopt;
        }
        else
        {
            statusParam = status_id;
        }



        LOG_DEBUG << "Filtering tasks with description containing: " << description;
        
        std::string nextMonday = WeekTimeUtils::getNextMonday();
        std::string nextFriday = WeekTimeUtils::getNextFriday();
        
        LOG_DEBUG << "Filtering this week's tasks with thisMonday: " << nextMonday << " and thisFriday: " << nextFriday;
        drogon::orm::Result result = clientPtr->execSqlSync(
            listThisWeekTasksSql,
            executorId,
            nextMonday,
            nextFriday
        );

        Json::Value tasksJson(Json::arrayValue);

        for (const drogon::orm::Row &row : result)
        {
            Json::Value taskJson;
            for (size_t i = 0; i < row.size(); i++)
            {
                const auto &field = row[i];
                const std::string colName = result.columnName(i);
                if (colName == "mentors" || colName == "executors")
                {
                    // JSON 字串需要解析
                    Json::Value parsed;
                    Json::Reader reader;
                    reader.parse(field.as<std::string>(), parsed);
                    taskJson[colName] = parsed;
                }
                else if (field.isNull())
                {
                    taskJson[colName] = Json::Value::null;
                }

                // 數值型欄位做特殊處理
                else if (colName == "progress" || colName == "excecutor_time_ratio")
                {
                    try
                    {
                        double val = std::stod(field.as<std::string>());
                        taskJson[colName] = val;
                    }
                    catch (...)
                    {
                        taskJson[colName] = 0.0;
                    }
                }
                else if (colName == "priority" || colName == "estimated_workdays")
                {
                    try
                    {
                        taskJson[colName] = std::stoi(field.as<std::string>());
                    }
                    catch (...)
                    {
                        taskJson[colName] = 0;
                    }
                }
                else if (colName == "completed" || colName == "delayed")
                {
                    // BOOLEAN 類型 (PostgreSQL 的 f/t)
                    std::string val = field.as<std::string>();
                    taskJson[colName] = (val == "t" || val == "true" || val == "1");
                }
                else
                {
                    taskJson[colName] = field.as<std::string>();
                }
            }
            tasksJson.append(taskJson);
        }

        Json::Value ret;
        ret["result"] = "ok";
        ret["tasks"] = tasksJson;
        ret["count"] = static_cast<int>(tasksJson.size());
        auto resp = HttpResponse::newHttpJsonResponse(ret);
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