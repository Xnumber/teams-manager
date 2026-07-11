#include "TaskStatusCtrl.h"
#include <jsoncpp/json/json.h>
#include <drogon/HttpResponse.h>
#include "models/TaskStatus.h"
#include "utils/mapToJson.h"
using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::teams_manager;

void TaskStatusCtrl::get(const HttpRequestPtr &req,
                        std::function<void(const HttpResponsePtr &)> &&callback, std::string id)
{
    LOG_DEBUG << "TaskStatus get called";
    DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
    orm::Mapper<TaskStatus> mapper(clientPtr);
    try
    {
        TaskStatus taskStatus = mapper.findByPrimaryKey(id);
        Json::Value ret;
        ret["result"] = "ok";
        ret["data"] = taskStatus.toJson();
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

void TaskStatusCtrl::list(const HttpRequestPtr &req,
                         std::function<void(const HttpResponsePtr &)> &&callback)
{
    try
    {
        DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
        orm::Mapper<TaskStatus> mapper(clientPtr);
        std::vector<TaskStatus> taskStatusList = mapper.findAll();
        Json::Value ret;
        ret["result"] = "ok";
        ret["data"] = mapToJson(taskStatusList, [](const auto &t) { return t.toJson(); });
        ret["count"] = static_cast<int>(taskStatusList.size());
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

void TaskStatusCtrl::create(const HttpRequestPtr &req,
                           std::function<void(const HttpResponsePtr &)> &&callback)
{
    LOG_DEBUG << "TaskStatus create called";
    DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
    orm::Mapper<TaskStatus> mapper(clientPtr);
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
        TaskStatus newTaskStatus(*json);
        mapper.insert(newTaskStatus);
        Json::Value ret;
        ret["result"] = "ok";
        ret["data"] = newTaskStatus.toJson();
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

void TaskStatusCtrl::update(const HttpRequestPtr &req,
                           std::function<void(const HttpResponsePtr &)> &&callback, std::string id)
{
    DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
    orm::Mapper<TaskStatus> mapper(clientPtr);
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
        TaskStatus taskStatus = mapper.findByPrimaryKey(id);
        taskStatus.updateByJson(*json);
        taskStatus.setId(id); // 保持主鍵不變
        mapper.update(taskStatus);
        Json::Value ret;
        ret["result"] = "ok";
        Json::Value arr(Json::arrayValue);
        arr.append(taskStatus.toJson());
        ret["data"] = arr;
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

void TaskStatusCtrl::remove(const HttpRequestPtr &req,
                           std::function<void(const HttpResponsePtr &)> &&callback, std::string id)
{
    LOG_DEBUG << "TaskStatus remove called";
    DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
    orm::Mapper<TaskStatus> mapper(clientPtr);
    try
    {
        TaskStatus taskStatus = mapper.findByPrimaryKey(id);
        mapper.deleteByPrimaryKey(id);
        Json::Value ret;
        ret["result"] = "ok";
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
