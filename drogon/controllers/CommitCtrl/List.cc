#include "CommitCtrl.h"
#include <jsoncpp/json/json.h>
#include "models/Commits.h"
using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::teams_manager;
void CommitCtrl::list(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
{
    auto dbClient = app().getDbClient("teams_manager");

    const std::string repoId = req->getParameter("repo_id");
    orm::Mapper<Commits> commitMapper(dbClient);
    try
    {
        Criteria criteria = Criteria(Commits::Cols::_github_repository_id, CompareOperator::EQ, repoId);
        std::vector<drogon_model::teams_manager::Commits> result = commitMapper.findBy(criteria);
        Json::Value data(Json::arrayValue);

        for (const auto& row : result)
        {
            data.append(row.toJson());
        }

        Json::Value ret;
        ret["result"] = "ok";
        ret["data"] = data;
        callback(HttpResponse::newHttpJsonResponse(ret));
    }
    catch (const std::exception& e)
    {
        Json::Value err;
        err["result"] = "error";
        err["message"] = e.what();
        auto resp = HttpResponse::newHttpJsonResponse(err);
        resp->setStatusCode(k500InternalServerError);
        callback(resp);
    }
}