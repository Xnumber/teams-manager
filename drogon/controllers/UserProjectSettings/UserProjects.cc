#include "UserProjectSettingCtrl.h"
#include "sql/get-user-projects.h"
#include <optional>

#include <drogon/HttpResponse.h>
#include <jsoncpp/json/json.h>

#include "models/UserProjectSettings.h"
#include "utils/mapToJson.h"



#include "plugins/RequestContext/RequestContext.h"

using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::teams_manager;


void UserProjectSettingCtrl::userProjects(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback)
{
    (void)req;
    LOG_DEBUG << "UserProjectSettingCtrl userProjects called";
    DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
    std::string userId = RequestContext::getUserId();
    
    LOG_DEBUG << "Fetching user projects for userId: " << userId;
    try
    {
        auto rows = clientPtr->execSqlSync(getUserProjectsSql, userId);

        Json::Value ret;
        ret["result"] = "ok";
        ret["data"] = mapToJson(
            rows,
            [](const auto &row)
            {
                Json::Value item;
                item["id"] = std::string(row["id"].c_str());
                item["name"] = std::string(row["name"].c_str());
                return item;
            });

        drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(ret);
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