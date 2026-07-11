#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class TeamsCtrl : public drogon::HttpController<TeamsCtrl>
{
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(TeamsCtrl::get, "/teams/{1}", Get, "AuthFilter");    // path is /teams/{teamId}
    ADD_METHOD_TO(TeamsCtrl::list, "/teams", Get);        // path is /teams
    // ADD_METHOD_TO(TeamsCtrl::create, "/teams", Post, "AuthFilter");     // path is /teams
    ADD_METHOD_TO(TeamsCtrl::create, "/teams", Post);     // path is /teams
    ADD_METHOD_TO(TeamsCtrl::update, "/teams/{1}", Put, "AuthFilter"); // path is /teams/{teamId}
    // ADD_METHOD_TO(TeamsCtrl::remove, "/teams/{1}", Delete); // path is /teams/{teamId}
    METHOD_LIST_END

    void get(const HttpRequestPtr &req,
             std::function<void(const HttpResponsePtr &)> &&callback, std::string teamId);
    void list(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback);
    void create(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback);
    void update(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback, std::string teamId);
    void remove(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback, std::string teamId);
};