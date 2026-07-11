#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class ProjectMetricsHistoryCtrl : public drogon::HttpController<ProjectMetricsHistoryCtrl>
{
public:
    METHOD_LIST_BEGIN

    ADD_METHOD_TO(ProjectMetricsHistoryCtrl::get,
                  "/project_metrics_history/{1}",
                  Get,
                  "AuthFilter");

    ADD_METHOD_TO(ProjectMetricsHistoryCtrl::list,
                  "/project_metrics_history",
                  Get);

    ADD_METHOD_TO(ProjectMetricsHistoryCtrl::create,
                  "/project_metrics_history",
                  Post);

    ADD_METHOD_TO(ProjectMetricsHistoryCtrl::update,
                  "/project_metrics_history/{1}",
                  Put,
                  "AuthFilter");

    ADD_METHOD_TO(ProjectMetricsHistoryCtrl::remove,
                  "/project_metrics_history/{1}",
                  Delete,
                  "AuthFilter");

    ADD_METHOD_TO(ProjectMetricsHistoryCtrl::updateTaskCountHistory,
                  "/project_metrics_history/update_task_count_history",
                  Put,
                  "AuthFilter");

    METHOD_LIST_END

    void get(const HttpRequestPtr &req,
             std::function<void(const HttpResponsePtr &)> &&callback,
             std::string id);

    void list(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback);

    void create(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback);

    void update(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback,
                std::string id);

    void remove(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback,
                std::string id);

    void updateTaskCountHistory(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback);
};