#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class PlanCtrl : public drogon::HttpController<PlanCtrl>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(PlanCtrl::get, "/plans/{1}", Get);        // path is /plans/{planId}
    ADD_METHOD_TO(PlanCtrl::list, "/plans", Get);           // path is /plans
    ADD_METHOD_TO(PlanCtrl::listMetricsHistories, "/plans/{1}/metrics_histories", Get);
    ADD_METHOD_TO(PlanCtrl::create, "/plans", Post);        // path is /plans
    ADD_METHOD_TO(PlanCtrl::update, "/plans/{1}", Put);     // path is /plans/{planId}
    ADD_METHOD_TO(PlanCtrl::updateMetricsHistory, "/plans/{1}/metrics_history", Put);
    ADD_METHOD_TO(PlanCtrl::remove, "/plans/{1}", Delete);  // path is /plans/{planId}
    METHOD_LIST_END

    void get(const HttpRequestPtr &req,
             std::function<void(const HttpResponsePtr &)> &&callback, std::string planId);
    void list(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback);
    void listMetricsHistories(const HttpRequestPtr &req,
                  std::function<void(const HttpResponsePtr &)> &&callback,
                  std::string planId);
    void create(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback);
    void update(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback, std::string planId);
    void updateMetricsHistory(const HttpRequestPtr &req,
                  std::function<void(const HttpResponsePtr &)> &&callback,
                  std::string planId);
    void remove(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback, std::string planId);
};
