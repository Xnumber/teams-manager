#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class TaskCtrl : public drogon::HttpController<TaskCtrl>
{
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(TaskCtrl::get, "/tasks/{1}", Get, "AuthFilter");    // path is /tasks/{taskId}
    ADD_METHOD_TO(TaskCtrl::list, "/tasks", Get, "AuthFilter");        // path is /tasks
    ADD_METHOD_TO(TaskCtrl::create, "/tasks", Post, "AuthFilter");     // path is /tasks
    ADD_METHOD_TO(TaskCtrl::update, "/tasks/{1}", Put, "AuthFilter"); // path is /tasks/{taskId}
    ADD_METHOD_TO(TaskCtrl::updateScheduledStartDate, "/tasks/scheduled-start-date", Put, "AuthFilter"); // path is /tasks/{taskId}
    // ADD_METHOD_TO(TaskCtrl::updateEndDate, "/tasks/{1}/endTime", Put, "AuthFilter"); // path is /tasks/{taskId}
    ADD_METHOD_TO(TaskCtrl::updateDependency, "/tasks/{1}/dependency", Put, "AuthFilter"); // path is /tasks/{taskId}/dependency
    ADD_METHOD_TO(TaskCtrl::removeDependency, "/tasks/delete-dependency", Put, "AuthFilter"); // path is /tasks/{taskId}/dependency
    ADD_METHOD_TO(TaskCtrl::removeScheduledStartDate, "/tasks/delete-scheduled-start-date", Delete, "AuthFilter"); // path is /tasks/{taskId}/dependency
    ADD_METHOD_TO(TaskCtrl::getDependency, "/tasks/{1}/dependency", Get, "AuthFilter"); // path is /tasks/{taskId}/dependency
    // ADD_METHOD_TO(TaskCtrl::remove, "/tasks/{1}", Delete, "AuthFilter"); // path is /tasks/{taskId}
    ADD_METHOD_TO(TaskCtrl::listFilter, "/tasks-filter", Get, "AuthFilter"); // path is /list-filter
    ADD_METHOD_TO(TaskCtrl::listThisWeek, "/this-week-tasks", Get, "AuthFilter"); // path is /this-week-tasks
    ADD_METHOD_TO(TaskCtrl::listNextWeek, "/next-week-tasks", Get, "AuthFilter"); // path is /next-week-tasks
    ADD_METHOD_TO(TaskCtrl::historyTasks, "/history-tasks", Get, "AuthFilter"); // path is /history-tasks
    ADD_METHOD_TO(TaskCtrl::completedTasks, "/completed-tasks", Get, "AuthFilter"); // path is /completed-tasks
    ADD_METHOD_TO(TaskCtrl::inCompleteTasks, "/incomplete-tasks", Get, "AuthFilter"); // path is /incomplete-tasks
    ADD_METHOD_TO(TaskCtrl::completedThisMonth, "/completed-this-month-tasks", Get, "AuthFilter"); // path is /completed-this-month-tasks
    ADD_METHOD_TO(TaskCtrl::completedTasksThisMonthByUser, "/completed-this-month-tasks-by-user", Get, "AuthFilter"); // path is /completed-this-month-tasks
    ADD_METHOD_TO(TaskCtrl::setTaskcompleted, "/set-task-completed/{1}", Put, "AuthFilter"); // path is /set-task-completed/{taskId}
    ADD_METHOD_TO(TaskCtrl::getParticipatingProjectsThisMonth, "/participating-projects-this-month", Get, "AuthFilter"); // path is /participating-project-count-this-month
    ADD_METHOD_TO(TaskCtrl::getParticipatingProjectsByMonth, "/participating-projects-by-month", Get, "AuthFilter");
    ADD_METHOD_TO(TaskCtrl::getCompletedTasksByMonth, "/task-completed-by-month", Get, "AuthFilter");
    ADD_METHOD_TO(TaskCtrl::getCompletedTasksByMonthAndUser, "/task-completed-by-month-and-user", Get, "AuthFilter");
    ADD_METHOD_TO(TaskCtrl::getParticipatingProjectsByMonthAndUser, "/participating-projects-by-month-and-user", Get, "AuthFilter");
    ADD_METHOD_TO(TaskCtrl::statistics, "/task-statistics", Get);
    ADD_METHOD_TO(TaskCtrl::reprioritize, "/task/reprioritize", Patch, "AuthFilter");
    ADD_METHOD_TO(TaskCtrl::reprioritize, "/task/reprioritize", Put, "AuthFilter");
    
    METHOD_LIST_END




    void get(const HttpRequestPtr &req,
             std::function<void(const HttpResponsePtr &)> &&callback, std::string taskId);
    void getDependency(const HttpRequestPtr &req,
             std::function<void(const HttpResponsePtr &)> &&callback, std::string taskId);
    void list(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback);
    void create(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback);
    void update(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback, std::string taskId);
    void updateDependency(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback, std::string taskId);
                
    void updateScheduledStartDate(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback);
    // void updateEndDate(const HttpRequestPtr &req,
    //             std::function<void(const HttpResponsePtr &)> &&callback, std::string taskId);
    void remove(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback, std::string taskId);
    void removeScheduledStartDate(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback);
    void removeDependency(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback);
    void listFilter(const HttpRequestPtr &req,
                    std::function<void(const HttpResponsePtr &)> &&callback);
    void listThisWeek(const HttpRequestPtr &req,
                      std::function<void(const HttpResponsePtr &)> &&callback);
    void listNextWeek(const HttpRequestPtr &req,
                      std::function<void(const HttpResponsePtr &)> &&callback);
    void historyTasks(const HttpRequestPtr &req,
                 std::function<void(const HttpResponsePtr &)> &&callback);
    void completedTasks(const HttpRequestPtr &req,
                 std::function<void(const HttpResponsePtr &)> &&callback);
    void inCompleteTasks(const HttpRequestPtr &req,
                 std::function<void(const HttpResponsePtr &)> &&callback);
    void completedThisMonth(const HttpRequestPtr &req,
                 std::function<void(const HttpResponsePtr &)> &&callback);
                 
    void completedTasksThisMonthByUser(const HttpRequestPtr &req,
                 std::function<void(const HttpResponsePtr &)> &&callback);


    
    void setTaskcompleted(const HttpRequestPtr &req,
                 std::function<void(const HttpResponsePtr &)> &&callback, std::string taskId);
    void getParticipatingProjectsThisMonth(const HttpRequestPtr &req,
                 std::function<void(const HttpResponsePtr &)> &&callback);
    void getParticipatingProjectsByMonth(const HttpRequestPtr &req,
                 std::function<void(const HttpResponsePtr &)> &&callback);
    void getCompletedTasksByMonth(const HttpRequestPtr &req,
                 std::function<void(const HttpResponsePtr &)> &&callback);
    void getCompletedTasksByMonthAndUser(const HttpRequestPtr &req,
                 std::function<void(const HttpResponsePtr &)> &&callback);
    void getParticipatingProjectsByMonthAndUser(const HttpRequestPtr &req,
                 std::function<void(const HttpResponsePtr &)> &&callback);
    void statistics(const HttpRequestPtr &req,
                 std::function<void(const HttpResponsePtr &)> &&callback);
    void reprioritize(const HttpRequestPtr &req,
                 std::function<void(const HttpResponsePtr &)> &&callback);
};