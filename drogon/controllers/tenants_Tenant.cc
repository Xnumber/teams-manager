#include "tenants_Tenant.h"
#include <jsoncpp/json/json.h>
#include <drogon/HttpResponse.h>
#include "models/Tenants.h"

using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::teams_manager;

#include "utils/mapToJson.h"

void Tenant::get(const HttpRequestPtr &req,
                    std::function<void(const HttpResponsePtr &)> &&callback, std::string tenantId)
{
    LOG_DEBUG << "Tenant get called";
    DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
    orm::Mapper<Tenants> mapper(clientPtr);
    try
    {        // 查找現有租戶
        Tenants tenant = mapper.findByPrimaryKey(tenantId);
        Json::Value ret;
        ret["result"] = "ok";
        ret["tenant"] = tenant.toJson();
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        callback(resp);
    } catch (const std::exception &e)
    {
        LOG_ERROR << "Error: " << e.what();
        // 如果是欄位不對，回應400 Bad Request
        Json::Value error;
        error["result"] = "error";
        error["message"] = e.what();

        auto resp = HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(k500InternalServerError);
        callback(resp);
    }

}

void Tenant::list(const HttpRequestPtr &req,
                  std::function<void(const HttpResponsePtr &)> &&callback)
{
    LOG_DEBUG << "Tenant list called";
    try
    {
        DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
        orm::Mapper<Tenants> mapper(clientPtr);

        // 查詢所有租戶
        std::vector<Tenants> tenantsList = mapper.findAll();
        Json::Value ret;
        ret["result"] = "ok";
        // ret["tenants"] = Json::Value(Json::arrayValue);
        // 遍歷結果並轉換為 JSON
        // for (const auto &tenant : tenantsList)
        // {
        //     // ret["tenants"].append(t);
        //     // 自定義格式的 JSON 轉換
        //     // Json::Value t;
        //     // t["id"] = tenant.getValueOfId();
        //     // t["name"] = tenant.getValueOfName();
        //     // t["description"] = tenant.getValueOfDescription();
        //     // t["concurrencyStamp"] = tenant.getValueOfConcurrencyStamp();
        //     // t["createdAt"] = tenant.getValueOfCreatedAt().toFormattedString(false);
        //     ret["tenants"].append(tenant.toJson());
        // }

        ret["tenants"] = mapToJson(tenantsList,
                                   [](const auto &t)
                                   { return t.toJson(); });

        ret["count"] = static_cast<int>(tenantsList.size());

        drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(ret);
        callback(resp);
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

void Tenant::create(const HttpRequestPtr &req,
                    std::function<void(const HttpResponsePtr &)> &&callback)
{
    LOG_DEBUG << "Tenant create called";
    DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
    orm::Mapper<Tenants> mapper(clientPtr);
    try
    {
        // 解析 JSON 請求體
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

        // 使用 Tenants 模型創建新租戶
        Tenants newTenant(*json);
        mapper.insert(newTenant);

        Json::Value ret;
        ret["result"] = "ok";
        ret["tenant"] = newTenant.toJson();

        drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(ret);
        callback(resp);
    }
    catch (const std::exception &e)
    {
        LOG_ERROR << "Error: " << e.what();
        // 如果是欄位不對，回應400 Bad Request
        Json::Value error;
        error["result"] = "error";
        error["message"] = e.what();

        drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(k500InternalServerError);
        callback(resp);
    }
}





void Tenant::update(const HttpRequestPtr &req,
                    std::function<void(const HttpResponsePtr &)> &&callback, std::string tenantId)
{
    LOG_DEBUG << "Tenant update called";
    DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
    orm::Mapper<Tenants> mapper(clientPtr);
    try
    {
        // 解析 JSON 請求體
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

        // 查找現有租戶
        Tenants tenant = mapper.findByPrimaryKey(tenantId);
        // LOG_DEBUG << "Tenant: " << tenant.toJson().toStyledString();
        tenant.updateByJson(*json);
        tenant.setConcurrencyStamp(drogon::utils::getUuid());
        mapper.update(tenant);

        Json::Value ret;
        ret["result"] = "ok";
        ret["tenant"] = tenant.toJson();
        
        // LOG_DEBUG << "Updated Tenant: " << tenant.toJson().toStyledString();

        auto resp = HttpResponse::newHttpJsonResponse(ret);
        callback(resp);
    }
    catch (const std::exception &e)
    {
        LOG_ERROR << "Error: " << e.what();
        // 如果是欄位不對，回應400 Bad Request
        Json::Value error;
        error["result"] = "error";
        error["message"] = e.what();

        auto resp = HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(k500InternalServerError);
        callback(resp);
    }
}

void Tenant::remove(const HttpRequestPtr &req,
                    std::function<void(const HttpResponsePtr &)> &&callback, std::string tenantId)
{
    LOG_DEBUG << "Tenant remove called";
    DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
    orm::Mapper<Tenants> mapper(clientPtr);
    // try
    // {
    //     // 查找現有租戶
    //     Tenants tenant = mapper.findByPrimaryKey(tenantId);
    //     mapper.deleteByPrimaryKey(tenantId);

    //     Json::Value ret;
    //     ret["result"] = "ok";
    //     ret["tenant"] = tenant.toJson();

    //     auto resp = HttpResponse::newHttpJsonResponse(ret);
    //     callback(resp);
    // }
    // catch (const std::exception &e)
    // {
        // LOG_ERROR << "Error: " << e.what();
        // 如果是欄位不對，回應400 Bad Request
        Json::Value error;
        error["result"] = "error";
        // error["message"] = e.what();
        error["message"] = "Internal Server Error";

        auto resp = HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(k500InternalServerError);
        callback(resp);
    // }
}