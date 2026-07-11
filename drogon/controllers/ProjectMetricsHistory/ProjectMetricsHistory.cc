#include "ProjectMetricsHistory.h"

#include <jsoncpp/json/json.h>
#include <drogon/HttpResponse.h>

#include "models/ProjectMetricsHistory.h"

using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::teams_manager;

void ProjectMetricsHistoryCtrl::get(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback,
    std::string id)
{
    LOG_DEBUG << "ProjectMetricsHistory get called";

    DbClientPtr clientPtr =
        drogon::app().getDbClient("teams_manager");

    orm::Mapper<ProjectMetricsHistory> mapper(clientPtr);

    Json::Value ret;

    try
    {
        auto record = mapper.findByPrimaryKey(id);

        ret["result"] = "ok";
        ret["data"] = record.toJson();

        auto resp = HttpResponse::newHttpJsonResponse(ret);
        callback(resp);
    }
    catch (const std::exception &e)
    {
        LOG_ERROR << e.what();

        ret["result"] = "error";
        ret["message"] = e.what();

        auto resp = HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(k500InternalServerError);

        callback(resp);
    }
}

void ProjectMetricsHistoryCtrl::list(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback)
{
    LOG_DEBUG << "ProjectMetricsHistory list called";
    DbClientPtr clientPtr =
        drogon::app().getDbClient("teams_manager");
    orm::Mapper<ProjectMetricsHistory> mapper(clientPtr);
    Json::Value ret;
    try
    {
        std::vector<drogon_model::teams_manager::ProjectMetricsHistory> records = mapper.findAll();

        ret["result"] = "ok";

        for (auto &record : records)
        {
            ret["data"].append(record.toJson());
        }

        drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(ret);
        callback(resp);
    }
    catch (const std::exception &e)
    {
        LOG_ERROR << e.what();

        ret["result"] = "error";
        ret["message"] = e.what();
        drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(k500InternalServerError);

        callback(resp);
    }
}

void ProjectMetricsHistoryCtrl::create(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback)
{
    LOG_DEBUG << "ProjectMetricsHistory create called";

    DbClientPtr clientPtr =
        drogon::app().getDbClient("teams_manager");

    orm::Mapper<ProjectMetricsHistory> mapper(clientPtr);

    Json::Value ret;

    try
    {
        std::shared_ptr<Json::Value> json = req->getJsonObject();

        if (!json)
        {
            ret["result"] = "error";
            ret["message"] = "Invalid JSON";

            auto resp = HttpResponse::newHttpJsonResponse(ret);
            resp->setStatusCode(k400BadRequest);

            callback(resp);
            return;
        }

        ProjectMetricsHistory record(*json);

        if (!json->isMember("changed_at"))
        {
            record.setChangedAt(trantor::Date::now());
        }

        mapper.insert(record);

        ret["result"] = "ok";
        ret["data"] = record.toJson();

        auto resp = HttpResponse::newHttpJsonResponse(ret);
        callback(resp);
    }
    catch (const std::exception &e)
    {
        LOG_ERROR << e.what();

        ret["result"] = "error";
        ret["message"] = e.what();

        drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(k500InternalServerError);

        callback(resp);
    }
}

void ProjectMetricsHistoryCtrl::update(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback,
    std::string id)
{
    LOG_DEBUG << "ProjectMetricsHistory update called";

    DbClientPtr clientPtr =
        drogon::app().getDbClient("teams_manager");

    orm::Mapper<ProjectMetricsHistory> mapper(clientPtr);

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

        auto record = mapper.findByPrimaryKey(id);

        if (json->isMember("date"))
            record.setDate(
                trantor::Date::fromDbString(
                    (*json)["date"].asString()));

        if (json->isMember("task_count"))
            record.setTaskCount(
                (*json)["task_count"].asInt());

        if (json->isMember("task_added_count"))
            record.setTaskAddedCount(
                (*json)["task_added_count"].asInt());

        if (json->isMember("task_completed_count"))
            record.setTaskCompletedCount(
                (*json)["task_completed_count"].asInt());

        if (json->isMember("milestone_count"))
            record.setMilestoneCount(
                (*json)["milestone_count"].asInt());

        if (json->isMember("milestone_added_count"))
            record.setMilestoneAddedCount(
                (*json)["milestone_added_count"].asInt());

        if (json->isMember("milestone_completed_count"))
            record.setMilestoneCompletedCount(
                (*json)["milestone_completed_count"].asInt());

        if (json->isMember("changed_at"))
            record.setChangedAt(
                trantor::Date::fromDbString(
                    (*json)["changed_at"].asString()));

        mapper.update(record);

        ret["result"] = "ok";
        ret["data"] = record.toJson();

        auto resp = HttpResponse::newHttpJsonResponse(ret);
        callback(resp);
    }
    catch (const std::exception &e)
    {
        LOG_ERROR << e.what();

        ret["result"] = "error";
        ret["message"] = e.what();

        auto resp = HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(k500InternalServerError);

        callback(resp);
    }
}

void ProjectMetricsHistoryCtrl::remove(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback,
    std::string id)
{
    LOG_DEBUG << "ProjectMetricsHistory remove called";

    DbClientPtr clientPtr =
        drogon::app().getDbClient("teams_manager");

    orm::Mapper<ProjectMetricsHistory> mapper(clientPtr);

    Json::Value ret;

    try
    {
        mapper.deleteByPrimaryKey(id);

        ret["result"] = "ok";

        auto resp = HttpResponse::newHttpJsonResponse(ret);
        callback(resp);
    }
    catch (const std::exception &e)
    {
        LOG_ERROR << e.what();

        ret["result"] = "error";
        ret["message"] = e.what();

        auto resp = HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(k500InternalServerError);

        callback(resp);
    }
}