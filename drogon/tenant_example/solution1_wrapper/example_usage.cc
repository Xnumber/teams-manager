/**
 * @file example_usage.cc
 * @brief TenantMapper 使用示例
 */

#include "TenantMapper.h"
#include "../models/Projects.h" // 假設 Projects 模型已生成
#include <drogon/HttpController.h>

using namespace drogon;
using namespace drogon_model::teams_manager;

/**
 * 示例 Controller：如何在 Controller 中使用 TenantMapper
 */
class ProjectController : public HttpController<ProjectController>
{
public:
    METHOD_LIST_BEGIN
    // 列出該租戶的所有項目
    ADD_METHOD_TO(ProjectController::list, "/api/projects", Get);
    // 獲取特定項目
    ADD_METHOD_TO(ProjectController::get, "/api/projects/{1}", Get);
    // 創建項目
    ADD_METHOD_TO(ProjectController::create, "/api/projects", Post);
    // 更新項目
    ADD_METHOD_TO(ProjectController::update, "/api/projects/{1}", Put);
    // 刪除項目
    ADD_METHOD_TO(ProjectController::remove, "/api/projects/{1}", Delete);
    METHOD_LIST_END

    /**
     * 列出租戶的所有項目
     */
    void list(const HttpRequestPtr& req,
              std::function<void(const HttpResponsePtr&)>&& callback)
    {
        // 從請求中獲取 tenant_id（假設已通過 Filter 設置）
        auto tenantId = req->attributes()->get<std::string>("tenant_id");
        
        DbClientPtr db = app().getDbClient();
        TenantMapper<Projects> mapper(db, tenantId);
        
        try
        {
            // 自動添加 tenant_id 條件的查詢
            auto projects = mapper
                .orderBy("created_at", SortOrder::DESC)
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

    /**
     * 獲取特定項目（自動驗證 tenant_id）
     */
    void get(const HttpRequestPtr& req,
             std::function<void(const HttpResponsePtr&)>&& callback,
             std::string projectId)
    {
        auto tenantId = req->attributes()->get<std::string>("tenant_id");
        DbClientPtr db = app().getDbClient();
        TenantMapper<Projects> mapper(db, tenantId);
        
        try
        {
            // findById 會自動驗證 tenant_id，防止跨租戶訪問
            auto project = mapper.findById(projectId);
            
            Json::Value ret;
            ret["result"] = "ok";
            ret["project"] = project.toJson();
            
            auto resp = HttpResponse::newHttpJsonResponse(ret);
            callback(resp);
        }
        catch (const UnexpectedRows& e)
        {
            // 記錄不存在或不屬於該租戶
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
     * 創建新項目（自動設置 tenant_id）
     */
    void create(const HttpRequestPtr& req,
                std::function<void(const HttpResponsePtr&)>&& callback)
    {
        auto tenantId = req->attributes()->get<std::string>("tenant_id");
        DbClientPtr db = app().getDbClient();
        TenantMapper<Projects> mapper(db, tenantId);
        
        try
        {
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
            
            // insert 會自動設置 tenant_id
            mapper.insert(newProject);
            
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

    /**
     * 更新項目（自動驗證 tenant_id）
     */
    void update(const HttpRequestPtr& req,
                std::function<void(const HttpResponsePtr&)>&& callback,
                std::string projectId)
    {
        auto tenantId = req->attributes()->get<std::string>("tenant_id");
        DbClientPtr db = app().getDbClient();
        TenantMapper<Projects> mapper(db, tenantId);
        
        try
        {
            // 先查詢確保記錄存在且屬於該租戶
            auto project = mapper.findById(projectId);
            
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
            
            // 更新欄位
            project.updateByJson(*json);
            
            // update 會驗證 tenant_id
            mapper.update(project);
            
            Json::Value ret;
            ret["result"] = "ok";
            ret["project"] = project.toJson();
            
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
     * 刪除項目（自動驗證 tenant_id）
     */
    void remove(const HttpRequestPtr& req,
                std::function<void(const HttpResponsePtr&)>&& callback,
                std::string projectId)
    {
        auto tenantId = req->attributes()->get<std::string>("tenant_id");
        DbClientPtr db = app().getDbClient();
        TenantMapper<Projects> mapper(db, tenantId);
        
        try
        {
            // 先查詢確保記錄存在且屬於該租戶
            auto project = mapper.findById(projectId);
            
            // deleteOne 會驗證 tenant_id
            mapper.deleteOne(project);
            
            Json::Value ret;
            ret["result"] = "ok";
            ret["message"] = "Project deleted successfully";
            
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
 * 進階使用示例
 */
void advancedUsageExamples()
{
    DbClientPtr db = app().getDbClient();
    std::string tenantId = "tenant-123";
    
    TenantMapper<Projects> mapper(db, tenantId);
    
    // 示例 1：鏈式調用 + 分頁
    auto projects = mapper
        .orderBy("created_at", SortOrder::DESC)
        .limit(10)
        .offset(0)
        .findAll();
    
    // 示例 2：複雜條件查詢
    auto activeProjects = mapper.findBy(
        Criteria(Projects::Cols::_test_count, CompareOperator::GT, 0)
    );
    // 實際 SQL: WHERE test_count > 0 AND tenant_id = 'tenant-123'
    
    // 示例 3：組合條件
    auto complexQuery = mapper.findBy(
        Criteria(Projects::Cols::_test_count, CompareOperator::GT, 10) &&
        Criteria(Projects::Cols::_unit_test_coverage, CompareOperator::IsNotNull)
    );
    // 實際 SQL: WHERE test_count > 10 AND unit_test_coverage IS NOT NULL AND tenant_id = 'tenant-123'
    
    // 示例 4：計數
    size_t totalProjects = mapper.count();
    size_t activeCount = mapper.count(
        Criteria(Projects::Cols::_test_count, CompareOperator::GT, 0)
    );
    
    // 示例 5：異步查詢
    mapper.findAll(
        [](std::vector<Projects> projects) {
            LOG_INFO << "Found " << projects.size() << " projects";
        },
        [](const DrogonDbException& e) {
            LOG_ERROR << "Database error: " << e.base().what();
        }
    );
    
    // 示例 6：Future 方式
    auto future = mapper.findFutureBy(
        Criteria(Projects::Cols::_name, CompareOperator::Like, "%test%")
    );
    auto results = future.get();
    
    // 示例 7：批量刪除（自動添加 tenant_id 條件）
    size_t deleted = mapper.deleteBy(
        Criteria(Projects::Cols::_test_count, CompareOperator::EQ, 0)
    );
    // 實際 SQL: DELETE FROM projects WHERE test_count = 0 AND tenant_id = 'tenant-123'
}
