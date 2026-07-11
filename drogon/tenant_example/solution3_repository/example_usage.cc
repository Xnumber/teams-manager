/**
 * @file example_usage.cc
 * @brief TenantRepository 使用示例
 */

#include "TenantRepository.h"
#include "../models/Projects.h"
#include <drogon/HttpController.h>
#include <drogon/HttpFilter.h>

using namespace drogon;
using namespace drogon_model::teams_manager;

/**
 * 示例：在 Filter 中設置 RequestContext
 */
class TenantAuthFilter : public HttpFilter<TenantAuthFilter>
{
public:
    void doFilter(const HttpRequestPtr& req,
                  FilterCallback&& fcb,
                  FilterChainCallback&& fccb) override
    {
        // 從 JWT 或其他來源獲取 tenant_id
        auto tenantId = extractTenantIdFromRequest(req);
        
        if (tenantId.empty())
        {
            Json::Value error;
            error["error"] = "Tenant ID not found";
            auto resp = HttpResponse::newHttpJsonResponse(error);
            resp->setStatusCode(k401Unauthorized);
            fcb(resp);
            return;
        }
        
        // 設置到 RequestContext
        RequestContext::setTenantId(tenantId);
        
        // 繼續處理請求
        fccb();
    }

private:
    std::string extractTenantIdFromRequest(const HttpRequestPtr& req)
    {
        // 從 JWT token 或 header 中提取 tenant_id
        // 這裡只是示例
        return req->getHeader("X-Tenant-ID");
    }
};

/**
 * 使用 TenantRepository 的 Controller 示例
 */
class ProjectControllerV3 : public HttpController<ProjectControllerV3>
{
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(ProjectControllerV3::list, "/v3/projects", Get, "TenantAuthFilter");
    ADD_METHOD_TO(ProjectControllerV3::get, "/v3/projects/{1}", Get, "TenantAuthFilter");
    ADD_METHOD_TO(ProjectControllerV3::create, "/v3/projects", Post, "TenantAuthFilter");
    METHOD_LIST_END

    /**
     * 列出項目 - 無需手動傳遞 tenant_id
     */
    void list(const HttpRequestPtr& req,
              std::function<void(const HttpResponsePtr&)>&& callback)
    {
        DbClientPtr db = app().getDbClient();
        
        try
        {
            // 直接創建 Repository，自動從 RequestContext 獲取 tenant_id
            TenantRepository<Projects> repo(db);
            
            // 使用非常簡潔
            auto projects = repo
                .orderBy("created_at", SortOrder::DESC)
                .limit(20)
                .findAll();
            
            Json::Value ret;
            ret["result"] = "ok";
            ret["tenant_id"] = repo.getTenantId(); // 可以獲取當前租戶 ID
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

    /**
     * 獲取單個項目
     */
    void get(const HttpRequestPtr& req,
             std::function<void(const HttpResponsePtr&)>&& callback,
             std::string projectId)
    {
        DbClientPtr db = app().getDbClient();
        
        try
        {
            TenantRepository<Projects> repo(db);
            
            // findById 自動驗證 tenant_id
            auto project = repo.findById(projectId);
            
            Json::Value ret;
            ret["result"] = "ok";
            ret["project"] = project.toJson();
            
            auto resp = HttpResponse::newHttpJsonResponse(ret);
            callback(resp);
        }
        catch (const UnexpectedRows& e)
        {
            Json::Value error;
            error["result"] = "error";
            error["message"] = "Project not found or access denied";
            
            auto resp = HttpResponse::newHttpJsonResponse(error);
            resp->setStatusCode(k404NotFound);
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

    /**
     * 創建項目
     */
    void create(const HttpRequestPtr& req,
                std::function<void(const HttpResponsePtr&)>&& callback)
    {
        DbClientPtr db = app().getDbClient();
        
        try
        {
            TenantRepository<Projects> repo(db);
            
            auto json = req->getJsonObject();
            if (!json)
            {
                Json::Value error;
                error["result"] = "error";
                error["message"] = "Invalid JSON body";
                
                auto resp = HttpResponse::newHttpJsonResponse(error);
                resp->setStatusCode(k400BadRequest);
                callback(resp);
                return;
            }
            
            Projects newProject(*json);
            
            // insert 自動設置 tenant_id
            repo.insert(newProject);
            
            Json::Value ret;
            ret["result"] = "ok";
            ret["project"] = newProject.toJson();
            
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
 * RequestContext 實現示例（需要放在項目中）
 */
// RequestContext.h
class RequestContext
{
public:
    static void setTenantId(const std::string& id)
    {
        tenantId_ = id;
    }
    
    static std::string getTenantId()
    {
        return tenantId_;
    }
    
    static void clear()
    {
        tenantId_.clear();
    }

private:
    static thread_local std::string tenantId_;
};

// RequestContext.cc
thread_local std::string RequestContext::tenantId_;

/**
 * 其他使用示例
 */
void repositoryExamples()
{
    DbClientPtr db = app().getDbClient();
    
    // RequestContext 已在 Filter 中設置
    TenantRepository<Projects> repo(db);
    
    // 簡潔的查詢
    auto allProjects = repo.findAll();
    
    // 帶條件的查詢
    auto activeProjects = repo.findBy(
        Criteria(Projects::Cols::_test_count, CompareOperator::GT, 0)
    );
    
    // 計數
    size_t total = repo.count();
    
    // 鏈式調用
    auto pagedProjects = repo
        .orderBy("name")
        .paginate(1, 10)
        .findAll();
    
    // 異步方式
    repo.findAll(
        [](std::vector<Projects> projects) {
            LOG_INFO << "Found " << projects.size() << " projects";
        },
        [](const DrogonDbException& e) {
            LOG_ERROR << e.base().what();
        }
    );
    
    // 批量刪除
    size_t deleted = repo.deleteBy(
        Criteria(Projects::Cols::_test_count, CompareOperator::EQ, 0)
    );
}
