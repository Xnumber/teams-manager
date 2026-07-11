#include "TaskTypeCtrl.h"
#include <jsoncpp/json/json.h>
#include <drogon/HttpResponse.h>
#include "models/TaskTypes.h"
#include "utils/mapToJson.h"
using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::teams_manager;

void TaskType::get(const HttpRequestPtr &req,
                   std::function<void(const HttpResponsePtr &)> &&callback, std::string id)
{
    LOG_DEBUG << "TaskType get called";
    DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
    orm::Mapper<TaskTypes> mapper(clientPtr);
    try
    {
        TaskTypes taskType = mapper.findByPrimaryKey(id);
        Json::Value ret;
        ret["result"] = "ok";
        ret["data"] = taskType.toJson();
        // LOG_DEBUG << "TaskType get result: " << ret.toStyledString();
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

void TaskType::list(const HttpRequestPtr &req,
                    std::function<void(const HttpResponsePtr &)> &&callback)
{
    // LOG_DEBUG << "TaskType list called";
    try
    {
        DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
        orm::Mapper<TaskTypes> mapper(clientPtr);
        std::vector<TaskTypes> taskTypesList = mapper.findAll();
        Json::Value ret;
        ret["result"] = "ok";
        ret["task_types"] = mapToJson(taskTypesList, [](const auto &t) { return t.toJson(); });
        ret["count"] = static_cast<int>(taskTypesList.size());
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

void TaskType::create(const HttpRequestPtr &req,
                      std::function<void(const HttpResponsePtr &)> &&callback)
{
    LOG_DEBUG << "TaskType create called";
    DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
    orm::Mapper<TaskTypes> mapper(clientPtr);
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
        TaskTypes newTaskType(*json);
        mapper.insert(newTaskType);
        Json::Value ret;
        ret["result"] = "ok";
        ret["data"] = newTaskType.toJson();
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

void TaskType::update(const HttpRequestPtr &req,
                      std::function<void(const HttpResponsePtr &)> &&callback, std::string id)
{
    // LOG_DEBUG << "TaskType update called";
    DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
    orm::Mapper<TaskTypes> mapper(clientPtr);
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
        TaskTypes taskType = mapper.findByPrimaryKey(id);
        taskType.updateByJson(*json);
        taskType.setConcurrencyStamp(drogon::utils::getUuid());
        mapper.update(taskType);
        Json::Value ret;
        ret["result"] = "ok";
        Json::Value arr(Json::arrayValue);
        arr.append(taskType.toJson());
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

void TaskType::remove(const HttpRequestPtr &req,
                      std::function<void(const HttpResponsePtr &)> &&callback, std::string id)
{
    LOG_DEBUG << "TaskType remove called";
    DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
    orm::Mapper<TaskTypes> mapper(clientPtr);
    try
    {
        TaskTypes taskType = mapper.findByPrimaryKey(id);
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
