#pragma once
#include <drogon/HttpController.h>
using namespace drogon;

class Milestone : public drogon::HttpController<Milestone>
{
public:
  METHOD_LIST_BEGIN
  ADD_METHOD_TO(Milestone::get, "/milestones/{1}", Get, "AuthFilter");    // path is /milestones/{milestoneId}
  ADD_METHOD_TO(Milestone::list, "/milestones", Get, "AuthFilter");        // path is /milestones
  ADD_METHOD_TO(Milestone::getCumulativeWorkingDayDelta, "/milestones/get-cumulative-working-day-delta", Get);     // path is /milestones
  ADD_METHOD_TO(Milestone::acceptanceTimeConvergence, "/milestone-acceptance-time-convergence", Get);     // path is /milestones
  ADD_METHOD_TO(Milestone::updateAcceptanceTimeConvergence, "/milestone-acceptance-time-convergence2", Put);     // path is /milestones
  ADD_METHOD_TO(Milestone::reprioritize, "/milestone/reprioritize", Patch, "AuthFilter");
  ADD_METHOD_TO(Milestone::reprioritize, "/milestone/reprioritize", Put, "AuthFilter");
  ADD_METHOD_TO(Milestone::create, "/milestones", Post, "AuthFilter");     // path is /milestones
  ADD_METHOD_TO(Milestone::update, "/milestones/{1}", Put, "AuthFilter"); // path is /milestones/{milestoneId}
  ADD_METHOD_TO(Milestone::remove, "/milestones/{1}", Delete, "AuthFilter"); // path is /milestones/{milestoneId}
  ADD_METHOD_TO(Milestone::complete, "/milestones/complete/{1}", Put, "AuthFilter"); // path is /milestones/{milestoneId}
  METHOD_LIST_END
  void get(const HttpRequestPtr &req,
           std::function<void(const HttpResponsePtr &)> &&callback, std::string milestoneId);
  void list(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback);
  void create(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback);
  void update(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback, std::string milestoneId);
  void remove(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback, std::string milestoneId);
  void acceptanceTimeConvergence(const HttpRequestPtr &req,
                                 std::function<void(const HttpResponsePtr &)> &&callback);
  void updateAcceptanceTimeConvergence(const HttpRequestPtr &req,
                                       std::function<void(const HttpResponsePtr &)> &&callback);
  void reprioritize(const HttpRequestPtr &req,
                    std::function<void(const HttpResponsePtr &)> &&callback);
  void getCumulativeWorkingDayDelta(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback);
  void complete(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback, std::string milestoneId);

private:
  std::string getString(const Json::Value &json, const char *key);
  std::string getIntAsString(const Json::Value &json, const char *key);
  std::string getBoolAsString(const Json::Value &json, const char *key);
};