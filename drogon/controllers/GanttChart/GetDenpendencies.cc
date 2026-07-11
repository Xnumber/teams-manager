#include "GanttChartCtrl.h"
#include "models/WorkItemDependencies.h"
#include "sql/sql.h"
#include "../../utils/date/date.h"
#include "utils/mapToJson.h"
#include <chrono>
#include <iomanip>
#include <sstream>
using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::teams_manager;

// Add definition of your processing function here
void GanttChartCtrl::getDependencies(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{

    // Your implementation here
    std::string project_id = req->getParameter("project_id");
    LOG_DEBUG << "Teams get called";
    DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
    orm::Mapper<WorkItemDependencies> mapper(clientPtr);

    try
    {
        std::vector<drogon_model::teams_manager::WorkItemDependencies> dependencies = mapper.findAll();
        Json::Value ret;
        ret["data"] = mapToJson(dependencies, [](const auto &t) { return t.toJson(); });
        drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(ret);
        callback(resp);
    }
    catch (const std::exception &e)
    {
        LOG_ERROR << "Error: " << e.what();
        Json::Value error;
        error["result"] = "error";
        error["message"] = e.what();
        drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(k500InternalServerError);
        callback(resp);
    }
}