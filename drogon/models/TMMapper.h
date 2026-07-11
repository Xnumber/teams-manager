/**
 * @file TMMapper.h
 * @brief 方案一：包裝類模式 - 自動添加 tenant_id 篩選條件
 *
 * 這是推薦的實現方式，通過包裝 Mapper 類來自動注入 tenant_id 查詢條件。
 *
 * 優點：
 * - 不修改 Drogon 源碼
 * - 類型安全
 * - 支持鏈式調用
 * - 可選擇性使用
 * - 易於測試和維護
 *
 * 使用示例：
 * @code
 * TMMapper<Projects> mapper(dbClient, tenantId);
 * auto projects = mapper.findAll();  // 自動添加 WHERE tenant_id = ?
 * @endcode
 */

#pragma once
#include <drogon/orm/Mapper.h>
#include <drogon/orm/Criteria.h>
#include "../plugins/RequestContext/RequestContext.h"
#include "Tenants.h"
#include "Users.h"
using namespace drogon::orm;

template <typename T>
class TMMapper
{
public:
    /**
     * @brief 構造函數
     * @param client 數據庫客戶端指針
     * @param tenantId 租戶 ID
     */
    TMMapper(DbClientPtr client): mapper_(client)
    // ,tenantId_(tenantId)
    {
        tenantId_ = RequestContext::getTenantId();
        LOG_DEBUG << "TMMapper constructor - Tenant ID from RequestContext: " << tenantId_;
        userId_ = RequestContext::getUserId();
        LOG_DEBUG << "TMMapper constructor - User ID from RequestContext: " << userId_;
        client_ = client;
    }

    /**
     * @brief 查詢所有記錄（自動添加 tenant_id 條件）
     * @return 符合條件的記錄向量
     */
    std::vector<T> findAll()
    {
        return mapper_.findBy(
            Criteria(T::Cols::_tenant_id, CompareOperator::EQ, tenantId_));
    }

    /**
     * @brief 根據條件查詢（在原有條件基礎上添加 tenant_id）
     * @param criteria 查詢條件
     * @return 符合條件的記錄向量
     */
    std::vector<T> findBy(const Criteria &criteria)
    {
        Criteria fullCriteria = criteria &&
                                Criteria(T::Cols::_tenant_id, CompareOperator::EQ, tenantId_);
        return mapper_.findBy(fullCriteria);
    }

    /**
     * @brief 查詢單條記錄（自動添加 tenant_id 條件）
     * @param criteria 查詢條件
     * @return 符合條件的記錄
     * @throws UnexpectedRows 如果沒有找到記錄或找到多條記錄
     */
    T findOne(const Criteria &criteria)
    {
        Criteria fullCriteria = criteria &&
                                Criteria(T::Cols::_tenant_id, CompareOperator::EQ, tenantId_);
        return mapper_.findOne(fullCriteria);
    }

    /**
     * @brief 通過主鍵查詢（自動添加 tenant_id 驗證）
     * @param id 主鍵 ID
     * @return 符合條件的記錄
     * @throws UnexpectedRows 如果記錄不存在或不屬於該租戶
     */
    T findById(const std::string &id)
    {
        auto result = mapper_.findBy(
            Criteria(T::Cols::_id, CompareOperator::EQ, id) &&
            Criteria(T::Cols::_tenant_id, CompareOperator::EQ, tenantId_));

        if (result.empty())
            throw UnexpectedRows("Record not found or access denied");

        return result.front();
    }

    // /**
    //  * @brief 計數（自動添加 tenant_id 條件）
    //  * @param criteria 查詢條件（可選）
    //  * @return 符合條件的記錄數
    //  */
    // size_t count(const Criteria& criteria = Criteria())
    // {
    //     Criteria fullCriteria = criteria.isNull() ?
    //         Criteria(T::Cols::_tenant_id, CompareOperator::EQ, tenantId_) :
    //         criteria && Criteria(T::Cols::_tenant_id, CompareOperator::EQ, tenantId_);
    //     return mapper_.count(fullCriteria);
    // }

    /**
     * @brief 異步查詢（自動添加 tenant_id 條件）
     * @param criteria 查詢條件
     * @param rcb 結果回調
     * @param ecb 異常回調
     */
    void findBy(const Criteria &criteria,
                const typename Mapper<T>::MultipleRowsCallback &rcb,
                const ExceptionCallback &ecb)
    {
        Criteria fullCriteria = criteria &&
                                Criteria(T::Cols::_tenant_id, CompareOperator::EQ, tenantId_);
        mapper_.findBy(fullCriteria, rcb, ecb);
    }

    /**
     * @brief 異步查詢所有記錄（自動添加 tenant_id 條件）
     * @param rcb 結果回調
     * @param ecb 異常回調
     */
    void findAll(const typename Mapper<T>::MultipleRowsCallback &rcb,
                 const ExceptionCallback &ecb)
    {
        findBy(Criteria(), rcb, ecb);
    }

    /**
     * @brief 異步查詢單條記錄（自動添加 tenant_id 條件）
     * @param criteria 查詢條件
     * @param rcb 結果回調
     * @param ecb 異常回調
     */
    void findOne(const Criteria &criteria,
                 const typename Mapper<T>::SingleRowCallback &rcb,
                 const ExceptionCallback &ecb)
    {
        Criteria fullCriteria = criteria &&
                                Criteria(T::Cols::_tenant_id, CompareOperator::EQ, tenantId_);
        mapper_.findOne(fullCriteria, rcb, ecb);
    }

    /**
     * @brief Future 方式查詢（自動添加 tenant_id 條件）
     * @param criteria 查詢條件
     * @return Future 對象
     */
    std::future<std::vector<T>> findFutureBy(const Criteria &criteria)
    {
        Criteria fullCriteria = criteria &&
                                Criteria(T::Cols::_tenant_id, CompareOperator::EQ, tenantId_);
        return mapper_.findFutureBy(fullCriteria);
    }

    /**
     * @brief 插入記錄（自動設置 tenant_id）
     * @param obj 要插入的對象
     */
    void insert(T &obj)
    {

        orm::Mapper<drogon_model::teams_manager::Tenants> tenantsMapper(client_);
        orm::Mapper<drogon_model::teams_manager::Users> usersMapper(client_);
        try
        {
            // LOG_DEBUG << "Inserting record for tenant: " << tenantId_;
            // LOG_DEBUG << "Inserting record for creator: " << userId_;
            drogon_model::teams_manager::Tenants tenant = tenantsMapper.findByPrimaryKey(tenantId_);
            drogon_model::teams_manager::Users user = usersMapper.findByPrimaryKey(userId_);
            std::string tenantName = tenant.getValueOfName();
            std::string userName = user.getValueOfUsername();

            obj.setTenantId(tenantId_);
            obj.setTenantName(tenantName);
            obj.setCreatorId(userId_);
            obj.setCreatorName(userName);
            mapper_.insert(obj);
        }
        catch (const std::exception &e)
        {
            LOG_ERROR << "Tenant - Inserting: " << e.what();
        }
    }

    // /**
    //  * @brief 異步插入記錄（自動設置 tenant_id）
    //  * @param obj 要插入的對象
    //  * @param rcb 結果回調
    //  * @param ecb 異常回調
    //  */
    // void insert(T obj,
    //             const typename Mapper<T>::SingleRowCallback& rcb,
    //             const ExceptionCallback& ecb)
    // {
    //     obj.setTenantId(tenantId_);
    //     mapper_.insert(obj, rcb, ecb);
    // }

    /**
     * @brief 更新記錄（驗證 tenant_id）
     * @param obj 要更新的對象
     * @return 更新的記錄數
     * @throws std::runtime_error 如果記錄不屬於該租戶
     */
    size_t update(const T &obj)
    {
        // 驗證記錄是否屬於當前租戶
        if (obj.getValueOfTenantId() != tenantId_)
            throw std::runtime_error("Cannot update record from different tenant");

        return mapper_.update(obj);
    }

    /**
     * @brief 刪除記錄（驗證 tenant_id）
     * @param obj 要刪除的對象
     * @return 刪除的記錄數
     * @throws std::runtime_error 如果記錄不屬於該租戶
     */
    size_t deleteOne(const T &obj)
    {
        // 驗證記錄是否屬於當前租戶
        if (obj.getValueOfTenantId() != tenantId_)
            throw std::runtime_error("Cannot delete record from different tenant");

        return mapper_.deleteOne(obj);
    }

    /**
     * @brief 根據條件刪除（自動添加 tenant_id 條件）
     * @param criteria 刪除條件
     * @return 刪除的記錄數
     */
    size_t deleteBy(const Criteria &criteria)
    {
        Criteria fullCriteria = criteria &&
                                Criteria(T::Cols::_tenant_id, CompareOperator::EQ, tenantId_);
        return mapper_.deleteBy(fullCriteria);
    }

    // ============ 鏈式調用支持 ============

    /**
     * @brief 設置查詢限制
     * @param limit 限制數量
     * @return TMMapper 引用（支持鏈式調用）
     */
    TMMapper<T> &limit(size_t limit)
    {
        mapper_.limit(limit);
        return *this;
    }

    /**
     * @brief 設置查詢偏移
     * @param offset 偏移量
     * @return TMMapper 引用（支持鏈式調用）
     */
    TMMapper<T> &offset(size_t offset)
    {
        mapper_.offset(offset);
        return *this;
    }

    /**
     * @brief 設置排序
     * @param colName 列名
     * @param order 排序方式
     * @return TMMapper 引用（支持鏈式調用）
     */
    TMMapper<T> &orderBy(const std::string &colName,
                             const SortOrder &order = SortOrder::ASC)
    {
        mapper_.orderBy(colName, order);
        return *this;
    }

    /**
     * @brief 設置排序（通過列索引）
     * @param colIndex 列索引
     * @param order 排序方式
     * @return TMMapper 引用（支持鏈式調用）
     */
    TMMapper<T> &orderBy(size_t colIndex,
                             const SortOrder &order = SortOrder::ASC)
    {
        mapper_.orderBy(colIndex, order);
        return *this;
    }

    /**
     * @brief 設置分頁
     * @param page 頁碼
     * @param perPage 每頁記錄數
     * @return TMMapper 引用（支持鏈式調用）
     */
    TMMapper<T> &paginate(size_t page, size_t perPage)
    {
        mapper_.paginate(page, perPage);
        return *this;
    }

    /**
     * @brief 鎖定查詢結果以供更新
     * @return TMMapper 引用（支持鏈式調用）
     */
    TMMapper<T> &forUpdate()
    {
        mapper_.forUpdate();
        return *this;
    }

    /**
     * @brief 獲取原始 Mapper（用於特殊場景）
     * @return 原始 Mapper 引用
     * @warning 使用原始 Mapper 將不會自動添加 tenant_id 條件
     */
    Mapper<T> &getMapper()
    {
        return mapper_;
    }

    /**
     * @brief 獲取當前租戶 ID
     * @return 租戶 ID
     */
    const std::string &getTenantId() const
    {
        return tenantId_;
    }

private:
    Mapper<T> mapper_;
    std::string tenantId_;
    std::string userId_;
    DbClientPtr client_;
};
