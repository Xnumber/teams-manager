#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class CommitCtrl : public drogon::HttpController<CommitCtrl>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(CommitCtrl::syncCommits, "/sync-commits", Post, "AuthFilter"); // path is /CommitCtrl/sync-commits
    ADD_METHOD_TO(CommitCtrl::syncCommitsAndUsers, "/sync-commits-and-users", Post, "AuthFilter"); // path is /CommitCtrl/sync-commits-and-users
    ADD_METHOD_TO(CommitCtrl::list, "/commits", Get, "AuthFilter"); // path is /CommitCtrl/commits
    // use METHOD_ADD to add your custom processing function here;
    // METHOD_ADD(CommitCtrl::get, "/{2}/{1}", Get); // path is /CommitCtrl/{arg2}/{arg1}
    // METHOD_ADD(CommitCtrl::your_method_name, "/{1}/{2}/list", Get); // path is /CommitCtrl/{arg1}/{arg2}/list
    // ADD_METHOD_TO(CommitCtrl::your_method_name, "/absolute/path/{1}/{2}/list", Get); // path is /absolute/path/{arg1}/{arg2}/list

    METHOD_LIST_END

    void syncCommits(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback);
    void syncCommitsAndUsers(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback);
    void list(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback);
    // your declaration of processing function maybe like this:
    // void get(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, int p1, std::string p2);
    // void your_method_name(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, double p1, int p2) const;
};
