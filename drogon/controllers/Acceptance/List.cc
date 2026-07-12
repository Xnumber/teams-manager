#include "AcceptanceCtrl.h"
#include <jsoncpp/json/json.h>
#include <drogon/HttpResponse.h>
#include "sql/sql.h"
#include "plugins/RequestContext/RequestContext.h"
#include "models/PlansProjects.h"





using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::teams_manager;

void AcceptanceCtrl::list(const HttpRequestPtr &req,
                          std::function<void(const HttpResponsePtr &)> &&callback)
{
    LOG_DEBUG << "Acceptance list called";
    try
    {
        DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");

        std::string requesterId = req->getParameter("requester_id");
        std::optional<std::string> requesterParam;
        if (requesterId.empty())
        {
            requesterParam = std::nullopt;
        }
        else
        {
            requesterParam = requesterId;
        }

        clientPtr->execSqlAsync(
            listTaskAcceptanceRequestsSql,
            [callback](const Result &r)
            {
                Json::Value ret;
                ret["result"] = "ok";

                Json::Value items(Json::arrayValue);
                for (const Row &row : r)
                {
                    Json::Value item(Json::objectValue);
                    for (size_t i = 0; i < row.size(); ++i)
                    {
                        const auto &field = row[i];
                        const std::string colName = r.columnName(i);
                        if (field.isNull())
                        {
                            item[colName] = Json::Value::null;
                        }
                        else
                        {
                            item[colName] = field.as<std::string>();
                        }
                    }
                    items.append(item);
                }

                ret["data"] = items;
                ret["count"] = static_cast<int>(items.size());
                auto resp = HttpResponse::newHttpJsonResponse(ret);
                callback(resp);
            },
            [callback](const DrogonDbException &e)
            {
                LOG_ERROR << "Database error: " << e.base().what();
                Json::Value error;
                error["result"] = "error";
                error["message"] = e.base().what();
                auto resp = HttpResponse::newHttpJsonResponse(error);
                resp->setStatusCode(k500InternalServerError);
                callback(resp);
            },
            requesterParam);
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
