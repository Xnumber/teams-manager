#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class TaskType : public drogon::HttpController<TaskType>
{
public:
  METHOD_LIST_BEGIN
  ADD_METHOD_TO(TaskType::get, "/task-types/{1}", Get);    // path is /task_types/{id}
  ADD_METHOD_TO(TaskType::list, "/task-types", Get);        // path is /task_types
  ADD_METHOD_TO(TaskType::create, "/task-types", Post);     // path is /task_types
  ADD_METHOD_TO(TaskType::update, "/task-types/{1}", Put); // path is /task_types/{id}
  ADD_METHOD_TO(TaskType::remove, "/task-types/{1}", Delete); // path is /task_types/{id}
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
