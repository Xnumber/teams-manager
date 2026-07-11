#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class RepositoryCtrl : public drogon::HttpController<RepositoryCtrl>
{
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(RepositoryCtrl::get, "/repositories/{1}", Get);    // path is /repositories/{id}
    ADD_METHOD_TO(RepositoryCtrl::list, "/repositories", Get);        // path is /repositories
    ADD_METHOD_TO(RepositoryCtrl::create, "/repositories", Post);     // path is /repositories
    // ADD_METHOD_TO(RepositoryCtrl::update, "/repositories/{1}", Put); // path is /repositories/{id}
    // ADD_METHOD_TO(RepositoryCtrl::remove, "/repositories/{1}", Delete); // path is /repositories/{id}
    METHOD_LIST_END

    void get(const HttpRequestPtr &req,
             std::function<void(const HttpResponsePtr &)> &&callback, std::string id);
    void list(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback);
    void create(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback);
    // void update(const HttpRequestPtr &req,
    //             std::function<void(const HttpResponsePtr &)> &&callback, std::string id);
    // void remove(const HttpRequestPtr &req,
    //             std::function<void(const HttpResponsePtr &)> &&callback, std::string id);
};
