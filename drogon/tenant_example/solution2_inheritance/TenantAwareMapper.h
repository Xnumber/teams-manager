/**
 * @file TenantAwareMapper.h
 * @brief 方案二：繼承模式 - 通過繼承 Mapper 自動添加 tenant_id 篩選條件
 * 
 * 這種方式通過繼承 Mapper 並重寫查詢方法來自動注入 tenant_id。
 * 
 * 優點：
 * - 完全兼容 Mapper API
 * - 可以重寫任意方法
 * 
 * 缺點：
 * - 繼承模板類較複雜
 * - 可能與未來 Drogon 版本不兼容
 * 
 * 使用示例：
 * @code
 * TenantAwareMapper<Projects> mapper(dbClient, tenantId);
 * auto projects = mapper.findAll();  // 自動添加 WHERE tenant_id = ?
 * @endcode
 */

#pragma once
#include <drogon/orm/Mapper.h>
#include <drogon/orm/Criteria.h>

using namespace drogon::orm;

template <typename T>
class TenantAwareMapper : public Mapper<T>
{
public:
    /**
     * @brief 構造函數
     * @param client 數據庫客戶端指針
     * @param tenantId 租戶 ID
     */
    TenantAwareMapper(DbClientPtr client, const std::string& tenantId)
        : Mapper<T>(client), tenantId_(tenantId)
    {
    }

    /**
     * @brief 重寫 findBy，自動注入 tenant_id
     */
    std::vector<T> findBy(const Criteria& criteria) noexcept(false) override
    {
        auto fullCriteria = addTenantCriteria(criteria);
        return Mapper<T>::findBy(fullCriteria);
    }

    /**
     * @brief 重寫 findAll，自動注入 tenant_id
     */
    std::vector<T> findAll() noexcept(false) override
    {
        return findBy(Criteria());
    }

    /**
     * @brief 重寫 findOne，自動注入 tenant_id
     */
    T findOne(const Criteria& criteria) noexcept(false) override
    {
        auto fullCriteria = addTenantCriteria(criteria);
        return Mapper<T>::findOne(fullCriteria);
    }

    /**
     * @brief 重寫 count，自動注入 tenant_id
     */
    size_t count(const Criteria& criteria = Criteria()) noexcept(false) override
    {
        auto fullCriteria = addTenantCriteria(criteria);
        return Mapper<T>::count(fullCriteria);
    }

    /**
     * @brief 重寫異步 findBy
     */
    void findBy(const Criteria& criteria,
                const typename Mapper<T>::MultipleRowsCallback& rcb,
                const ExceptionCallback& ecb) noexcept override
    {
        auto fullCriteria = addTenantCriteria(criteria);
        Mapper<T>::findBy(fullCriteria, rcb, ecb);
    }

    /**
     * @brief 重寫異步 findAll
     */
    void findAll(const typename Mapper<T>::MultipleRowsCallback& rcb,
                 const ExceptionCallback& ecb) noexcept override
    {
        findBy(Criteria(), rcb, ecb);
    }

    /**
     * @brief 重寫異步 findOne
     */
    void findOne(const Criteria& criteria,
                 const typename Mapper<T>::SingleRowCallback& rcb,
                 const ExceptionCallback& ecb) noexcept override
    {
        auto fullCriteria = addTenantCriteria(criteria);
        Mapper<T>::findOne(fullCriteria, rcb, ecb);
    }

    /**
     * @brief 重寫 Future findBy
     */
    std::future<std::vector<T>> findFutureBy(const Criteria& criteria) noexcept override
    {
        auto fullCriteria = addTenantCriteria(criteria);
        return Mapper<T>::findFutureBy(fullCriteria);
    }

    /**
     * @brief 重寫 Future findAll
     */
    std::future<std::vector<T>> findFutureAll() noexcept override
    {
        return findFutureBy(Criteria());
    }

    /**
     * @brief 重寫 Future findOne
     */
    std::future<T> findFutureOne(const Criteria& criteria) noexcept override
    {
        auto fullCriteria = addTenantCriteria(criteria);
        return Mapper<T>::findFutureOne(fullCriteria);
    }

    /**
     * @brief 重寫 deleteBy，自動注入 tenant_id
     */
    size_t deleteBy(const Criteria& criteria) noexcept(false) override
    {
        auto fullCriteria = addTenantCriteria(criteria);
        return Mapper<T>::deleteBy(fullCriteria);
    }

    /**
     * @brief 重寫 insert，自動設置 tenant_id
     */
    void insert(T& obj) noexcept(false) override
    {
        obj.setTenantId(tenantId_);
        Mapper<T>::insert(obj);
    }

    /**
     * @brief 獲取租戶 ID
     */
    const std::string& getTenantId() const
    {
        return tenantId_;
    }

private:
    std::string tenantId_;

    /**
     * @brief 添加 tenant_id 條件到現有條件
     */
    Criteria addTenantCriteria(const Criteria& original) const
    {
        Criteria tenantCrit(T::Cols::_tenant_id, CompareOperator::EQ, tenantId_);
        
        if (original.isNull())
            return tenantCrit;
        
        return original && tenantCrit;
    }
};
