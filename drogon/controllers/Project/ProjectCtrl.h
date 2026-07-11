#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class Project : public drogon::HttpController<Project>
{
public:
  METHOD_LIST_BEGIN
  ADD_METHOD_TO(Project::get, "/projects/{1}", Get, "AuthFilter");    // path is /projects/{projectId}
  ADD_METHOD_TO(Project::list, "/projects", Get, "AuthFilter");        // path is /projects
  ADD_METHOD_TO(Project::listForAnalytics, "/projects/analytics", Get, "AuthFilter");        // path is /projects/analytics
  // ADD_METHOD_TO(Project::list, "/projects", Get);        // path is /projects
  ADD_METHOD_TO(Project::getCumulativeWorkingDayDelta, "/projects/get-cumulative-working-day-delta", Get);        // compatibility path
  // ADD_METHOD_TO(Project::getCumulativeWorkingDayDelta, "/projects/cumulative-working-day-delta", Get);        // path is /projects/cumulative-working-day-delta
  ADD_METHOD_TO(Project::create, "/projects", Post, "AuthFilter");     // path is /projects
  ADD_METHOD_TO(Project::update, "/projects/{1}", Put, "AuthFilter"); // path is /projects/{projectId}
  ADD_METHOD_TO(Project::remove, "/projects/{1}", Delete, "AuthFilter"); // path is /projects/{projectId}
  ADD_METHOD_TO(Project::acceptanceTimeConvergence, "/project-acceptance-time-convergence", Get, "AuthFilter"); // path is /projects/{projectId}
  ADD_METHOD_TO(Project::updateAcceptanceTimeConvergence, "/project-acceptance-time-convergence", Put, "AuthFilter"); // path is /projects/{projectId}
  ADD_METHOD_TO(Project::reprioritize, "/project/reprioritize", Patch, "AuthFilter"); // path is /projects/{projectId}
  ADD_METHOD_TO(Project::reprioritize, "/project/reprioritize", Put, "AuthFilter"); // compatibility for clients still using PUT
  ADD_METHOD_TO(Project::getParticipantsInfo, "/projects/{1}/participants-info", Get, "AuthFilter"); // path is /projects/{projectId}/participants-info
  ADD_METHOD_TO(Project::getOverview, "/projects/{1}/overview", Get, "AuthFilter"); // path is /projects/{projectId}/overview







  METHOD_LIST_END
  void get(const HttpRequestPtr &req,
           std::function<void(const HttpResponsePtr &)> &&callback, std::string projectId);
  void list(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback);
  void listForAnalytics(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback);
  void getCumulativeWorkingDayDelta(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback);
  void create(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback);
  void update(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback, std::string projectId);
  void remove(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback, std::string projectId);
  void acceptanceTimeConvergence(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback);
  void updateAcceptanceTimeConvergence(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback);
  void reprioritize(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback);
  void getParticipantsInfo(const HttpRequestPtr &req,
           std::function<void(const HttpResponsePtr &)> &&callback, std::string projectId);
  void getOverview(const HttpRequestPtr &req,
           std::function<void(const HttpResponsePtr &)> &&callback, std::string projectId);
};