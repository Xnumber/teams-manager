#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class PullRequestCtrl : public drogon::HttpController<PullRequestCtrl>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(PullRequestCtrl::syncPullRequests, "/sync-pull-requests", Post, "AuthFilter"); // path is /PullRequestCtrl/{arg1}/{arg2}
    ADD_METHOD_TO(PullRequestCtrl::syncPullRequestsAndUsers, "/sync-pull-requests-and-users", Post, "AuthFilter"); // path is /PullRequestCtrl/{arg1}/{arg2}
    ADD_METHOD_TO(PullRequestCtrl::list, "/pull-requests", Get, "AuthFilter"); // path is /PullRequestCtrl/{arg1}/{arg2}
    // use METHOD_ADD to add your custom processing function here;
    // METHOD_ADD(PullRequestCtrl::get, "/{2}/{1}", Get); // path is /PullRequestCtrl/{arg2}/{arg1}
    // METHOD_ADD(PullRequestCtrl::your_method_name, "/{1}/{2}/list", Get); // path is /PullRequestCtrl/{arg1}/{arg2}/list
    // ADD_METHOD_TO(PullRequestCtrl::your_method_name, "/absolute/path/{1}/{2}/list", Get); // path is /absolute/path/{arg1}/{arg2}/list

    METHOD_LIST_END
    // your declaration of processing function maybe like this:
    // void get(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, int p1, std::string p2);
    // void your_method_name(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, double p1, int p2) const;
    void syncPullRequests(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback);
    void syncPullRequestsAndUsers(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback);
    void list(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback);
};
