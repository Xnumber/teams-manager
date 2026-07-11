#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class PlanHistoryCtrl : public drogon::HttpController<PlanHistoryCtrl>
{
  public:
    METHOD_LIST_BEGIN
    // use METHOD_ADD to add your custom processing function here;
    // METHOD_ADD(PlanHistoryCtrl::get, "/{2}/{1}", Get); // path is /PlanHistoryCtrl/{arg2}/{arg1}
    // METHOD_ADD(PlanHistoryCtrl::your_method_name, "/{1}/{2}/list", Get); // path is /PlanHistoryCtrl/{arg1}/{arg2}/list
    ADD_METHOD_TO(PlanHistoryCtrl::planHistory, "/plan-history/{1}", Get); // path is /absolute/path/{arg1}/{arg2}/list

    METHOD_LIST_END
    // your declaration of processing function maybe like this:
    // void get(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, int p1, std::string p2);
    // void your_method_name(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, double p1, int p2) const;

    void planHistory(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, const std::string &planIdStr);
};
