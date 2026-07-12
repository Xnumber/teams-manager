#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class AcceptanceCtrl : public drogon::HttpController<AcceptanceCtrl>
{
  public:
    METHOD_LIST_BEGIN

    // use METHOD_ADD to add your custom processing function here;
    // METHOD_ADD(BussinesCalendarCtrl::get, "/{2}/{1}", Get); // path is /BussinesCalendarCtrl/{arg2}/{arg1}
    // METHOD_ADD(BussinesCalendarCtrl::your_method_name, "/{1}/{2}/list", Get); // path is /BussinesCalendarCtrl/{arg1}/{arg2}/list
    ADD_METHOD_TO(AcceptanceCtrl::list, "/acceptances", Get); // path is /import-business-calendar
    ADD_METHOD_TO(AcceptanceCtrl::createRequest, "/acceptances", Post); // path is /import-business-calendar

    METHOD_LIST_END
    // your declaration of processing function maybe like this:
    // void get(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, int p1, std::string p2);
    // void your_method_name(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, double p1, int p2) const;
    void createRequest(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback);
    void list(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback);
};
