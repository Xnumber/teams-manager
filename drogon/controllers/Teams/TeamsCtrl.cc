#include "TeamsCtrl.h"
#include <jsoncpp/json/json.h>
#include <drogon/HttpResponse.h>
#include "models/Teams.h"
#include "models/Tenants.h"
#include "controllers/Teams/sql/list.h"
using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::teams_manager;

void TeamsCtrl::get(const HttpRequestPtr &req,
                    std::function<void(const HttpResponsePtr &)> &&callback, std::string teamId)
{
    LOG_DEBUG << "Teams get called";
    DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
    orm::Mapper<Teams> mapper(clientPtr);
    try
    {
        Teams team = mapper.findByPrimaryKey(teamId);
        Json::Value ret;
        ret["result"] = "ok";
        ret["data"] = team.toJson();
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

void TeamsCtrl::list(const HttpRequestPtr &req,
                     std::function<void(const HttpResponsePtr &)> &&callback)
{
    LOG_DEBUG << "Teams list called";
    DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
    try
    {
        std::string tenantName = req->getParameter("tenant_name");
        Json::Value ret;
        ret["result"] = "ok";

        if (!tenantName.empty())
        {
            auto result = clientPtr->execSqlSync(
                "SELECT * FROM (" + getTeamsListSql + ") teams_list WHERE tenant_name = $1",
                tenantName);

            for (const auto &row : result)
            {
                Json::Value item;
                item["concurrency_stamp"] = row["concurrency_stamp"].as<std::string>();
                item["description"] = row["description"].isNull() ? "" : row["description"].as<std::string>();
                item["tasks_count_in_queue"] = row["tasks_count_in_queue"].as<int>();
                item["left_task"] = row["left_task"].as<int>();
                item["id"] = row["id"].as<std::string>();
                item["name"] = row["name"].as<std::string>();
                item["tenant_id"] = row["tenant_id"].as<std::string>();
                item["tenant_name"] = row["tenant_name"].as<std::string>();
                ret["data"].append(item);
            }

            auto resp = HttpResponse::newHttpJsonResponse(ret);
            callback(resp);
        }
        else
        {
            auto result = clientPtr->execSqlSync(getTeamsListSql);

            for (const auto &row : result)
            {
                Json::Value item;
                item["concurrency_stamp"] = row["concurrency_stamp"].as<std::string>();
                item["description"] = row["description"].isNull() ? "" : row["description"].as<std::string>();
                item["tasks_count_in_queue"] = row["tasks_count_in_queue"].as<int>();
                item["left_task"] = row["left_task"].as<int>();
                item["id"] = row["id"].as<std::string>();
                item["name"] = row["name"].as<std::string>();
                item["tenant_id"] = row["tenant_id"].as<std::string>();
                item["tenant_name"] = row["tenant_name"].as<std::string>();
                ret["data"].append(item);
            }

            auto resp = HttpResponse::newHttpJsonResponse(ret);
            callback(resp);
        }
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

void TeamsCtrl::create(const HttpRequestPtr &req,
                       std::function<void(const HttpResponsePtr &)> &&callback)
{
    LOG_DEBUG << "Teams create called";
    DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
    orm::Mapper<Teams> mapper(clientPtr);
    orm::Mapper<Tenants> tenantsMapper(clientPtr);
    Json::Value ret;
    try
    {
        auto json = req->getJsonObject();
        if (!json)
        {
            ret["result"] = "error";
            ret["message"] = "Invalid JSON";
            auto resp = HttpResponse::newHttpJsonResponse(ret);
            resp->setStatusCode(k400BadRequest);
            callback(resp);
            return;
        }
        
        
        if (json->isMember("tenant_id") && (*json)["tenant_id"].isString())
        {
            
            try
            {
                Tenants tenant = tenantsMapper.findByPrimaryKey((*json)["tenant_id"].asString());
                (*json)["tenant_name"] = tenant.getValueOfName();
            }
            catch (const std::exception &e)
            {
                LOG_ERROR << "Tenant fetch error: " << e.what();
            }
        }
        Teams team(*json);
        team.setCreatedAt(trantor::Date::now());
        // LOG_DEBUG << "Team JSON: " << (*json).toStyledString();
        mapper.insert(team);
        ret["result"] = "ok";
        ret["data"] = team.toJson();
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        callback(resp);
    }
    catch (const std::exception &e)
    {
        LOG_ERROR << "Error: " << e.what();
        ret["result"] = "error";
        ret["message"] = e.what();
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(k500InternalServerError);
        callback(resp);
    }
}

void TeamsCtrl::update(const HttpRequestPtr &req,
                       std::function<void(const HttpResponsePtr &)> &&callback, std::string teamId)
{
    LOG_DEBUG << "Teams update called";
    DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
    orm::Mapper<Teams> mapper(clientPtr);
    Json::Value ret;
    try
    {
        auto json = req->getJsonObject();
        if (!json)
        {
            ret["result"] = "error";
            ret["message"] = "Invalid JSON";
            auto resp = HttpResponse::newHttpJsonResponse(ret);
            resp->setStatusCode(k400BadRequest);
            callback(resp);
            return;
        }
        Teams team = mapper.findByPrimaryKey(teamId);
        if ((*json).isMember("name"))
            team.setName((*json)["name"].asString());
        if ((*json).isMember("description"))
            team.setDescription((*json)["description"].asString());
        if ((*json).isMember("concurrency_stamp"))
            team.setConcurrencyStamp((*json)["concurrency_stamp"].asString());
        mapper.update(team);
        ret["result"] = "ok";
        ret["data"] = team.toJson();
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        callback(resp);
    }
    catch (const std::exception &e)
    {
        LOG_ERROR << "Error: " << e.what();
        ret["result"] = "error";
        ret["message"] = e.what();
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(k500InternalServerError);
        callback(resp);
    }
}

void TeamsCtrl::remove(const HttpRequestPtr &req,
                       std::function<void(const HttpResponsePtr &)> &&callback, std::string teamId)
{
    LOG_DEBUG << "Teams remove called";
    DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
    orm::Mapper<Teams> mapper(clientPtr);
    Json::Value ret;
    try
    {
        mapper.deleteByPrimaryKey(teamId);
        ret["result"] = "ok";
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        callback(resp);
    }
    catch (const std::exception &e)
    {
        LOG_ERROR << "Error: " << e.what();
        ret["result"] = "error";
        ret["message"] = e.what();
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(k500InternalServerError);
        callback(resp);
    }
}
