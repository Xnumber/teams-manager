#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class GanttChartCtrl : public drogon::HttpController<GanttChartCtrl>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(GanttChartCtrl::getGanttChartData, "/gantt-chart", Get); // path is /GanttChartCtrl/{arg1}/{arg2}
    ADD_METHOD_TO(GanttChartCtrl::getCorrectedGanttChartData, "/corrected-gantt-chart", Get); // path is /GanttChartCtrl/{arg1}/{arg2}
    ADD_METHOD_TO(GanttChartCtrl::getDependencies, "/gantt-chart/dependencies", Get); // path is /GanttChartCtrl/{arg1}/{arg2}
    // ADD_METHOD_TO(GanttChartCtrl::get2, "/gantt-chart-2", Get); // path is /GanttChartCtrl/{arg1}/{arg2}
    // use METHOD_ADD to add your custom processing function here;
    // METHOD_ADD(GanttChartCtrl::get, "/{2}/{1}", Get); // path is /GanttChartCtrl/{arg2}/{arg1}
    // METHOD_ADD(GanttChartCtrl::your_method_name, "/{1}/{2}/list", Get); // path is /GanttChartCtrl/{arg1}/{arg2}/list
    // ADD_METHOD_TO(GanttChartCtrl::your_method_name, "/absolute/path/{1}/{2}/list", Get); // path is /absolute/path/{arg1}/{arg2}/list

    METHOD_LIST_END
    // your declaration of processing function maybe like this:
    // void get(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, int p1, std::string p2);
    // void your_method_name(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, double p1, int p2) const;
    void getGanttChartData(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback);
    void getCorrectedGanttChartData(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback);
    void getDependencies(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback);
    // void get2(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback);
};
