#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class User : public drogon::HttpController<User>
{
public:
  METHOD_LIST_BEGIN
  METHOD_ADD(User::get, "/{1}", Get, "AuthFilter");    // path is /users/User/{userId}
  METHOD_ADD(User::list, "", Get, "AuthFilter");        // path is /users/User
  METHOD_ADD(User::create, "", Post, "AuthFilter");     // path is /users/User
  METHOD_ADD(User::update, "/{1}", Put, "AuthFilter"); // path is /users/User/{userId}
  // METHOD_ADD(User::remove, "/{1}", Delete, "AuthFilter"); // path is /users/User/{userId}
  METHOD_LIST_END

  void get(const HttpRequestPtr &req,
           std::function<void(const HttpResponsePtr &)> &&callback, std::string userId);
  void list(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback);
  void create(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback);
  void update(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback, std::string userId);
  void remove(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback, std::string userId);
};
