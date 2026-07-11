#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class TaskStatusCtrl : public drogon::HttpController<TaskStatusCtrl>
{
public:
  METHOD_LIST_BEGIN
  ADD_METHOD_TO(TaskStatusCtrl::get, "/task-status/{1}", Get);    // path is /task-status/{id}
  ADD_METHOD_TO(TaskStatusCtrl::list, "/task-status", Get);        // path is /task-status
  ADD_METHOD_TO(TaskStatusCtrl::create, "/task-status", Post);     // path is /task-status
  ADD_METHOD_TO(TaskStatusCtrl::update, "/task-status/{1}", Put); // path is /task-status/{id}
  ADD_METHOD_TO(TaskStatusCtrl::remove, "/task-status/{1}", Delete); // path is /task-status/{id}
  METHOD_LIST_END

  void get(const HttpRequestPtr &req,
           std::function<void(const HttpResponsePtr &)> &&callback, std::string id);
  void list(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback);
  void create(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback);
  void update(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback, std::string id);
  void remove(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback, std::string id);
};
