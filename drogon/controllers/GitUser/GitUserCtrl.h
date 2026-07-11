#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class GitUserCtrl : public drogon::HttpController<GitUserCtrl>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(GitUserCtrl::bindGithubUser, "/bind-github-user", Post, "AuthFilter");
    METHOD_LIST_END
    void bindGithubUser(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback);
};
