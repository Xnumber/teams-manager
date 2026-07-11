#pragma once
#include <drogon/HttpController.h>

using namespace drogon;


class StatisticCtrl : public drogon::HttpController<StatisticCtrl>
{
  public:
    METHOD_LIST_BEGIN
    // use METHOD_ADD to add your custom processing function here;
    ADD_METHOD_TO(StatisticCtrl::statisticGitInfoThisMonth, "/statistic-git-info-this-month", Get); // path is /StatisticCtrl/{arg1}/{arg2}
    // ADD_METHOD_TO(StatisticCtrl::statisticGitInfoByMonthAndUser, "/statistic-git-info-by-month-and-user", Get); // path is /StatisticCtrl/{arg1}/{arg2}
    ADD_METHOD_TO(StatisticCtrl::statisticGitInfoByMonth, "/statistic-git-info-by-month", Get); // path is /StatisticCtrl/{arg1}/{arg2}
    ADD_METHOD_TO(StatisticCtrl::statisticGitInfoByMonthAndUser, "/statistic-git-info-by-month-and-user", Get); // path is /StatisticCtrl/{arg1}/{arg2}
    ADD_METHOD_TO(StatisticCtrl::statisticByMonth, "/statistic-by-month", Get); // path is /StatisticCtrl/{arg1}/{arg2}
    // METHOD_ADD(StatisticCtrl::get, "/{2}/{1}", Get); // path is /StatisticCtrl/{arg2}/{arg1}
    // METHOD_ADD(StatisticCtrl::your_method_name, "/{1}/{2}/list", Get); // path is /StatisticCtrl/{arg1}/{arg2}/list
    // ADD_METHOD_TO(StatisticCtrl::your_method_name, "/absolute/path/{1}/{2}/list", Get); // path is /absolute/path/{arg1}/{arg2}/list
    METHOD_LIST_END
    void statisticGitInfoThisMonth(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback);
    void statisticGitInfoByMonth(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback);
    void statisticGitInfoByMonthAndUser(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback);
    void statisticByMonth(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback);
    // your declaration of processing function maybe like this:
    // void get(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, int p1, std::string p2);
    // void your_method_name(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, double p1, int p2) const;
};