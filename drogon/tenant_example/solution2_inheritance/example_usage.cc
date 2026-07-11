/**
 * @file example_usage.cc
 * @brief TenantAwareMapper 使用示例
 */

#include "TenantAwareMapper.h"
#include "../models/Projects.h"
#include <drogon/HttpController.h>

using namespace drogon;
using namespace drogon_model::teams_manager;

/**
 * 使用繼承方式的 Controller 示例
 */
class ProjectControllerV2 : public HttpController<ProjectControllerV2>
{
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(ProjectControllerV2::listProjects, "/v2/projects", Get);
    METHOD_LIST_END

    void listProjects(const HttpRequestPtr& req,
                      std::function<void(const HttpResponsePtr&)>&& callback)
    {
        auto tenantId = req->attributes()->get<std::string>("tenant_id");
        DbClientPtr db = app().getDbClient();
        
        // 使用 TenantAwareMapper 替代 Mapper
        TenantAwareMapper<Projects> mapper(db, tenantId);
        
        try
        {
            // 完全兼容 Mapper API，使用方式相同
            auto projects = mapper
                .orderBy("created_at", SortOrder::DESC)
                .limit(20)
                .findAll();
            
            Json::Value ret;
            ret["result"] = "ok";
            ret["projects"] = Json::Value(Json::arrayValue);
            
            for (const auto& project : projects)
            {
                ret["projects"].append(project.toJson());
            }
            
            auto resp = HttpResponse::newHttpJsonResponse(ret);
            callback(resp);
        }
        catch (const std::exception& e)
        {
            LOG_ERROR << "Error: " << e.what();
            Json::Value error;
            error["result"] = "error";
            error["message"] = e.what();
            
            auto resp = HttpResponse::newHttpJsonResponse(error);
            resp->setStatusCode(k500InternalServerError);
            callback(resp);
        }
    }
};

/**
 * 示例：與原始 Mapper 完全相同的使用方式
 */
void inheritanceExamples()
{
    DbClientPtr db = app().getDbClient();
    std::string tenantId = "tenant-456";
    
    // 使用方式與 Mapper 完全相同
    TenantAwareMapper<Projects> mapper(db, tenantId);
    
    // 所有查詢自動添加 tenant_id 條件
    auto allProjects = mapper.findAll();
    
    auto filtered = mapper.findBy(
        Criteria(Projects::Cols::_test_count, CompareOperator::GT, 5)
    );
    
    size_t total = mapper.count();
    
    // 支持鏈式調用
    auto paged = mapper
        .orderBy("name")
        .limit(10)
        .offset(20)
        .findAll();
    
    // 異步方式
    mapper.findAll(
        [](std::vector<Projects> projects) {
            LOG_INFO << "Found " << projects.size() << " projects";
        },
        [](const DrogonDbException& e) {
            LOG_ERROR << e.base().what();
        }
    );
}
