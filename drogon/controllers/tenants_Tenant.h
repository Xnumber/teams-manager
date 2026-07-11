#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

// namespace tenants
// {
// namespace v1
// {

class Tenant : public drogon::HttpController<Tenant>
{
public:
  METHOD_LIST_BEGIN
  // use METHOD_ADD to add your custom processing function here;
  // METHOD_ADD(Tenant::get, "/{2}/{1}", Get); // path is /tenants/Tenant/{arg2}/{arg1}
  METHOD_ADD(Tenant::get, "/{1}", Get);    // path is /tenants/Tenant/{arg2}/{arg1}
  METHOD_ADD(Tenant::list, "", Get);    // path is /tenants/Tenant/{arg2}/{arg1}
  METHOD_ADD(Tenant::create, "", Post); // path is /tenants/Tenant/{arg2}/{arg1}
  METHOD_ADD(Tenant::update, "/{1}", Put);  // path is /tenants/Tenant/{arg2}/{arg1}
  METHOD_ADD(Tenant::remove, "/{1}", Delete);  // path is /tenants/Tenant/{arg2}/{arg1}
  // METHOD_ADD(Tenant::your_method_name, "/{1}/{2}/list", Get); // path is /tenants/Tenant/{arg1}/{arg2}/list
  // ADD_METHOD_TO(Tenant::your_method_name, "/absolute/path/{1}/{2}/list", Get); // path is /absolute/path/{arg1}/{arg2}/list

  METHOD_LIST_END
  // your declaration of processing function maybe like this:
  // void get(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, int p1, std::string p2);
  // void your_method_name(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, double p1, int p2) const;
  void get(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback, std::string tenantId);
  void list(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback);
  void create(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback);
  void update(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback, std::string tenantId);
  void remove(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback, std::string tenantId);
};
// }
// }