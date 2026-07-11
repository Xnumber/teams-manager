#include "PlanCtrl.h"
#include <jsoncpp/json/json.h>
#include <drogon/HttpResponse.h>
#include "models/Plans.h"
#include "sql/sql.h"
#include <sstream>
using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::teams_manager;

void PlanCtrl::get(const HttpRequestPtr &req,
                   std::function<void(const HttpResponsePtr &)> &&callback, std::string planId)
{
    LOG_DEBUG << "Plan get called";
    DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
    orm::Mapper<Plans> mapper(clientPtr);
    try
    {
        Plans plan = mapper.findByPrimaryKey(planId);
        Json::Value ret;
        ret["result"] = "ok";
        ret["data"] = plan.toJson();
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


void PlanCtrl::list(const HttpRequestPtr &req,
                    std::function<void(const HttpResponsePtr &)> &&callback)
{
    LOG_DEBUG << "Plan list called";
    DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
    Json::Value ret;
    try
    {
        std::future<drogon::orm::Result> resultFuture =
            clientPtr->execSqlAsyncFuture(listPlansSql);

        std::thread([callback, resultFuture = std::move(resultFuture), ret]() mutable {
            try
            {
                drogon::orm::Result r = resultFuture.get();

                ret["result"] = "ok";
                for (const auto &row : r)
                {
                    Json::Value planData;
                    planData["id"] = row["id"].as<std::string>();
                    planData["name"] = row["name"].as<std::string>();
                    planData["description"] = row["description"].as<std::string>();
                    planData["start_date"] = row["start_date"].as<std::string>();
                    planData["end_date"] = row["end_date"].as<std::string>();
                    planData["created_at"] = row["created_at"].as<std::string>();
                    planData["status"] = row["status"].as<std::string>();

                    // Parse project_ids JSON array
                    std::string projectIdsJson = row["project_ids"].as<std::string>();
                    Json::Value projectIds;
                    Json::CharReaderBuilder builder;
                    std::string errs;
                    std::istringstream iss(projectIdsJson);
                    if (!Json::parseFromStream(builder, iss, &projectIds, &errs))
                    {
                        projectIds = Json::arrayValue;
                    }
                    planData["project_ids"] = projectIds;

                    ret["data"].append(planData);
                }

                drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(ret);
                callback(resp);
            }
            catch (const std::exception &e)
            {
                LOG_ERROR << "Error: " << e.what();
                ret["result"] = "error";
                ret["message"] = e.what();
                drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(ret);
                resp->setStatusCode(k500InternalServerError);
                callback(resp);
            }
        }).detach();
    }
    catch (const std::exception &e)
    {
        LOG_ERROR << "Error: " << e.what();
        ret["result"] = "error";
        ret["message"] = e.what();
        drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(k500InternalServerError);
        callback(resp);
    }
}


void PlanCtrl::create(const HttpRequestPtr &req,
                      std::function<void(const HttpResponsePtr &)> &&callback)
{
    LOG_DEBUG << "Plan create called";
    DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
    Json::Value ret;
    try
    {
        const std::shared_ptr<Json::Value> json = req->getJsonObject();
        if (!json)
        {
            ret["result"] = "error";
            ret["message"] = "Invalid JSON";
            auto resp = HttpResponse::newHttpJsonResponse(ret);
            resp->setStatusCode(k400BadRequest);
            callback(resp);
            return;
        }

        // Validate required fields
        if (!json->isMember("name") || !json->isMember("description") ||
            !json->isMember("start_date") || !json->isMember("end_date"))
        {
            ret["result"] = "error";
            ret["message"] = "Missing required fields: name, description, start_date, end_date";
            auto resp = HttpResponse::newHttpJsonResponse(ret);
            resp->setStatusCode(k400BadRequest);
            callback(resp);
            return;
        }

        // Prepare project_ids as PostgreSQL ARRAY format
        std::string projectIdsArray;
        if ((*json).isMember("project_ids") && (*json)["project_ids"].isArray())
        {
            projectIdsArray = "{";
            const Json::Value &projectIds = (*json)["project_ids"];
            for (size_t i = 0; i < projectIds.size(); ++i)
            {
                if (i > 0) projectIdsArray += ",";
                projectIdsArray += projectIds[(int)i].asString();
            }
            projectIdsArray += "}";
        }

        std::string name = (*json)["name"].asString();
        std::string description = (*json)["description"].asString();
        std::string startDate = (*json)["start_date"].asString();
        std::string endDate = (*json)["end_date"].asString();
        std::string status = (*json).isMember("status") ? (*json)["status"].asString() : "";

        std::future<drogon::orm::Result> resultFuture =
            clientPtr->execSqlAsyncFuture(createPlansSql,
                                         name,
                                         description,
                                         startDate,
                                         endDate,
                                         status,
                                         projectIdsArray);

        std::thread([callback, resultFuture = std::move(resultFuture), ret]() mutable {
            try
            {
                drogon::orm::Result r = resultFuture.get();
                if (r.size() == 0)
                {
                    ret["result"] = "error";
                    ret["message"] = "Failed to create plan";
                    auto resp = HttpResponse::newHttpJsonResponse(ret);
                    resp->setStatusCode(k500InternalServerError);
                    callback(resp);
                    return;
                }

                Json::Value planData;
                const auto &row = r[0];
                planData["id"] = row["id"].as<std::string>();
                planData["name"] = row["name"].as<std::string>();
                planData["description"] = row["description"].as<std::string>();
                planData["start_date"] = row["start_date"].as<std::string>();
                planData["end_date"] = row["end_date"].as<std::string>();
                planData["created_at"] = row["created_at"].as<std::string>();
                planData["status"] = row["status"].as<std::string>();

                ret["result"] = "ok";
                ret["data"] = planData;
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
        }).detach();
    }
    catch (const std::exception &e)
    {
        LOG_ERROR << "Error: " << e.what();
        ret["result"] = "error";
        ret["message"] = e.what();
        drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(k500InternalServerError);
        callback(resp);
    }
}





void PlanCtrl::update(const HttpRequestPtr &req,
                      std::function<void(const HttpResponsePtr &)> &&callback, std::string planId)
{
    LOG_DEBUG << "Plan update called";
    DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
    Json::Value ret;
    try
    {
        const std::shared_ptr<Json::Value> json = req->getJsonObject();
        if (!json)
        {
            ret["result"] = "error";
            ret["message"] = "Invalid JSON";
            drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(ret);
            resp->setStatusCode(k400BadRequest);
            callback(resp);
            return;
        }

        // Prepare project_ids as PostgreSQL ARRAY format
        std::string projectIdsArray;
        if ((*json).isMember("project_ids") && (*json)["project_ids"].isArray())
        {
            projectIdsArray = "{";
            const Json::Value &projectIds = (*json)["project_ids"];
            for (size_t i = 0; i < projectIds.size(); ++i)
            {
                if (i > 0) projectIdsArray += ",";
                projectIdsArray += projectIds[(int)i].asString();
            }
            projectIdsArray += "}";
        }

        std::string name = (*json).isMember("name") ? (*json)["name"].asString() : "";
        std::string description = (*json).isMember("description") ? (*json)["description"].asString() : "";
        std::string startDate = (*json).isMember("start_date") ? (*json)["start_date"].asString() : "";
        std::string endDate = (*json).isMember("end_date") ? (*json)["end_date"].asString() : "";
        std::string status = (*json).isMember("status") ? (*json)["status"].asString() : "";

        std::future<drogon::orm::Result> resultFuture =
            clientPtr->execSqlAsyncFuture(updatePlansSql,
                                         planId,
                                         projectIdsArray,
                                         name,
                                         description,
                                         startDate,
                                         endDate,
                                         status);

        std::thread([callback, resultFuture = std::move(resultFuture), ret]() mutable {
            try
            {
                drogon::orm::Result r = resultFuture.get();
                if (r.size() == 0)
                {
                    ret["result"] = "error";
                    ret["message"] = "Plan not found";
                    auto resp = HttpResponse::newHttpJsonResponse(ret);
                    resp->setStatusCode(k404NotFound);
                    callback(resp);
                    return;
                }

                Json::Value planData;
                const auto &row = r[0];
                planData["id"] = row["id"].as<std::string>();
                planData["name"] = row["name"].as<std::string>();
                planData["description"] = row["description"].as<std::string>();
                planData["start_date"] = row["start_date"].as<std::string>();
                planData["end_date"] = row["end_date"].as<std::string>();
                planData["created_at"] = row["created_at"].as<std::string>();
                planData["status"] = row["status"].as<std::string>();

                ret["result"] = "ok";
                ret["data"] = planData;
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
        }).detach();
    }
    catch (const std::exception &e)
    {
        LOG_ERROR << "Error: " << e.what();
        ret["result"] = "error";
        ret["message"] = e.what();
        drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(k500InternalServerError);
        callback(resp);
    }
}

void PlanCtrl::remove(const HttpRequestPtr &req,
                      std::function<void(const HttpResponsePtr &)> &&callback, std::string planId)
{
    LOG_DEBUG << "Plan remove called";
    DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
    orm::Mapper<Plans> mapper(clientPtr);
    Json::Value ret;
    try
    {
        mapper.deleteByPrimaryKey(planId);
        ret["result"] = "ok";
        drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(ret);
        callback(resp);
    }
    catch (const std::exception &e)
    {
        LOG_ERROR << "Error: " << e.what();
        ret["result"] = "error";
        ret["message"] = e.what();
        drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(k500InternalServerError);
        callback(resp);
    }
}
