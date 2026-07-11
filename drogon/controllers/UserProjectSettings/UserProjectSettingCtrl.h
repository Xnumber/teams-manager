#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class UserProjectSettingCtrl : public drogon::HttpController<UserProjectSettingCtrl>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(UserProjectSettingCtrl::get, "/user-project-settings/{1}", Get);
    ADD_METHOD_TO(UserProjectSettingCtrl::userProjects, "/user-projects", Get);
    ADD_METHOD_TO(UserProjectSettingCtrl::list, "/user-project-settings", Get);
    ADD_METHOD_TO(UserProjectSettingCtrl::create, "/user-project-settings", Post);
    ADD_METHOD_TO(UserProjectSettingCtrl::update, "/user-project-settings/{1}", Put);
    ADD_METHOD_TO(UserProjectSettingCtrl::remove, "/user-project-settings/{1}", Delete);

    METHOD_LIST_END
    void get(const HttpRequestPtr &req,
             std::function<void(const HttpResponsePtr &)> &&callback,
             std::string id);
    void list(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback);
    void userProjects(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback);
    void create(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback);
    void update(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback,
                std::string id);
    void remove(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback,
                std::string id);
};
