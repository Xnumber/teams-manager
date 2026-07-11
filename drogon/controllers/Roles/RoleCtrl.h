#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class RoleCtrl : public drogon::HttpController<RoleCtrl>
{
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(RoleCtrl::get, "/roles/{1}", Get, "AuthFilter");    // path is /roles/{roleId}
    ADD_METHOD_TO(RoleCtrl::list, "/roles", Get, "AuthFilter");        // path is /roles
    ADD_METHOD_TO(RoleCtrl::create, "/roles", Post, "AuthFilter");     // path is /roles
    ADD_METHOD_TO(RoleCtrl::update, "/roles/{1}", Put, "AuthFilter"); // path is /roles/{roleId}
    ADD_METHOD_TO(RoleCtrl::remove, "/roles/{1}", Delete, "AuthFilter"); // path is /roles/{roleId}
    METHOD_LIST_END

    void get(const HttpRequestPtr &req,
             std::function<void(const HttpResponsePtr &)> &&callback, std::string roleId);
    void list(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback);
    void create(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback);
    void update(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback, std::string roleId);
    void remove(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback, std::string roleId);
};
