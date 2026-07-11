#include "UserCtrl.h"
#include <jsoncpp/json/json.h>
#include <drogon/HttpResponse.h>
#include "models/Users.h"
#include "models/Tenants.h"
#include "utils/mapToJson.h"
#include "sql/list.h"
using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::teams_manager;


void User::get(const HttpRequestPtr &req,
               std::function<void(const HttpResponsePtr &)> &&callback, std::string userId)
{
    // LOG_DEBUG << "User get called";
    DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
    orm::Mapper<Users> mapper(clientPtr);
    try
    {
        Users user = mapper.findByPrimaryKey(userId);
        Json::Value ret;
        ret["result"] = "ok";
        ret["data"] = user.toJson();
        ret["data"]["name"] = user.getValueOfUsername();
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        callback(resp);
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

void User::list(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback)
{
    LOG_DEBUG << "User list called";
    try
    {
        DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
        clientPtr->execSqlAsync(
            getUserListSql,
            [callback](const drogon::orm::Result &r) {
                Json::Value ret;
                ret["result"] = "ok";
                Json::Value data(Json::arrayValue);
                for (const auto &row : r)
                {
                    Json::Value userJson;
                    userJson["id"] = row["id"].as<std::string>();
                    userJson["username"] = row["username"].as<std::string>();
                    userJson["left_tasks"] = row["left_tasks"].as<int>();
                    userJson["tenant_name"] = row["tenant_name"].as<std::string>();
                    userJson["tenant_id"] = row["tenant_id"].as<std::string>();
                    userJson["team_name"] = row["team_name"].as<std::string>();
                    userJson["team_id"] = row["team_id"].as<std::string>();
                    userJson["project_count"] = row["project_count"].as<int>();
                    data.append(userJson);
                }
                ret["data"] = data;
                auto resp = HttpResponse::newHttpJsonResponse(ret);
                callback(resp);
            },
            [callback](const DrogonDbException &e) {
                LOG_ERROR << "Database error: " << e.base().what();
                Json::Value error;
                error["result"] = "error";
                error["message"] = e.base().what();
                auto resp = HttpResponse::newHttpJsonResponse(error);
                resp->setStatusCode(k500InternalServerError);
                callback(resp);
            });
    

        // orm::Mapper<Users> mapper(clientPtr);
        // std::vector<Users> usersList = mapper.findAll();
        // Json::Value ret;
        // ret["result"] = "ok";
        // // 只選擇指定欄位
        
        // ret["data"] = mapToJson(usersList, [](const auto &u) {
        //     Json::Value j;
        //     j["id"] = u.getValueOfId();
        //     j["name"] = u.getValueOfUsername();
        //     j["tenant_id"] = u.getValueOfTenantId();
        //     j["tenant_name"] = u.getValueOfTenantName();
        //     j["team_id"] = u.getValueOfTeamId();
        //     j["team_name"] = u.getValueOfTeamName();
        //     j["email"] = u.getValueOfEmail();
        //     return j;
        // });
        // ret["count"] = static_cast<int>(usersList.size());
        // drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(ret);
        // callback(resp);
    }
    catch (const DrogonDbException &e)
    {
        LOG_ERROR << "Database error: " << e.base().what();
        Json::Value error;
        error["result"] = "error";
        error["message"] = e.base().what();
        auto resp = HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(k500InternalServerError);
        callback(resp);
    }
}

void User::create(const HttpRequestPtr &req,
                  std::function<void(const HttpResponsePtr &)> &&callback)
{
    LOG_DEBUG << "User create called";
    try
    {
        DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
        orm::Mapper<Users> mapper(clientPtr);
        orm::Mapper<Tenants> tenantMapper(clientPtr);

        
        auto json = req->getJsonObject();
        std::string tenant_Id = json->get("tenant_id", "").asString();
        Tenants tenant = tenantMapper.findByPrimaryKey(tenant_Id);
        if (!json)
        {
            Json::Value error;
            error["result"] = "error";
            error["message"] = "Invalid JSON body";
            auto resp = HttpResponse::newHttpJsonResponse(error);
            resp->setStatusCode(k400BadRequest);
            callback(resp);
            return;
        }
        Users newUser(*json);
        newUser.setTenantName(tenant.getValueOfName());
        mapper.insert(newUser);
        Json::Value ret;
        ret["result"] = "ok";
        ret["data"] = newUser.toJson();
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        callback(resp);
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

void User::update(const HttpRequestPtr &req,
                  std::function<void(const HttpResponsePtr &)> &&callback, std::string userId)
{
    LOG_DEBUG << "User update called";
    DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
    orm::Mapper<Users> mapper(clientPtr);
    try
    {
        auto json = req->getJsonObject();
        if (!json)
        {
            Json::Value error;
            error["result"] = "error";
            error["message"] = "Invalid JSON body";
            auto resp = HttpResponse::newHttpJsonResponse(error);
            resp->setStatusCode(k400BadRequest);
            callback(resp);
            return;
        }
        Users user = mapper.findByPrimaryKey(userId);
        user.updateByJson(*json);
        user.setConcurrencyStamp(drogon::utils::getUuid());
        mapper.update(user);
        Json::Value ret;
        ret["result"] = "ok";
        ret["data"] = user.toJson();
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        callback(resp);
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

void User::remove(const HttpRequestPtr &req,
                  std::function<void(const HttpResponsePtr &)> &&callback, std::string userId)
{
    LOG_DEBUG << "User remove called";
    DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
    orm::Mapper<Users> mapper(clientPtr);
    try
    {
        Users user = mapper.findByPrimaryKey(userId);
        mapper.deleteByPrimaryKey(userId);
        Json::Value ret;
        ret["result"] = "ok";
        ret["data"] = user.toJson();
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        callback(resp);
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