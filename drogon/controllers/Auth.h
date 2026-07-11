#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class Auth : public drogon::HttpController<Auth>
{
public:
  METHOD_LIST_BEGIN
  // ADD_METHOD_TO(Auth::get, "/projects/{1}", Get);    // path is /projects/{projectId}
  // ADD_METHOD_TO(Auth::list, "/projects", Get);        // path is /projects
  ADD_METHOD_TO(Auth::registerUser, "/register", Post);     // path is /register
  ADD_METHOD_TO(Auth::loginUser, "/login", Post);     // path is /login
  // ADD_METHOD_TO(Auth::update, "/projects/{1}", Put); // path is /projects/{projectId}
  // ADD_METHOD_TO(Auth::remove, "/projects/{1}", Delete); // path is /projects/{projectId}
  METHOD_LIST_END


  void registerUser(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback);

  void loginUser(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback);

};
