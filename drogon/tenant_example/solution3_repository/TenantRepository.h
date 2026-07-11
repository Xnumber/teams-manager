// /**
//  * @file TenantRepository.h
//  * @brief 方案三：基於 RequestContext 的 Repository 模式
//  * 
//  * 這種方式從 RequestContext 自動獲取 tenant_id，無需手動傳遞。
//  * 
//  * 優點：
//  * - 自動從請求上下文獲取 tenant_id
//  * - 減少參數傳遞
//  * - 適合 DI 架構
//  * 
//  * 缺點：
//  * - 依賴全局狀態（RequestContext）
//  * - 測試時需要 mock RequestContext
//  * 
//  * 前置條件：
//  * - 需要在 Filter 中設置 RequestContext::setTenantId()
//  * 
//  * 使用示例：
//  * @code
//  * TenantRepository<Projects> repo(dbClient);  // 自動從 RequestContext 獲取 tenant_id
//  * auto projects = repo.findAll();
//  * @endcode
//  */

// #pragma once
// #include <drogon/orm/Mapper.h>
// #include <drogon/orm/Criteria.h>

// using namespace drogon::orm;

// /**
//  * @brief RequestContext 類（需要在項目中實現）
//  * 
//  * 示例實現：
//  * @code
//  * class RequestContext
//  * {
//  * public:
//  *     static void setTenantId(const std::string& id) { tenantId_ = id; }
//  *     static std::string getTenantId() { return tenantId_; }
//  * private:
//  *     static thread_local std::string tenantId_;
//  * };
//  * thread_local std::string RequestContext::tenantId_;
//  * @endcode
//  */
// class RequestContext
// {
// public:
//     static void setTenantId(const std::string& id);
//     static std::string getTenantId();
// };

// template <typename T>
// class TenantRepository
// {
// public:
//     /**
//      * @brief 構造函數（自動從 RequestContext 獲取 tenant_id）
//      * @param client 數據庫客戶端指針
//      * @throws std::runtime_error 如果 RequestContext 中沒有設置 tenant_id
//      */
//     explicit TenantRepository(DbClientPtr client)
//         : mapper_(client)
//     {
//         tenantId_ = RequestContext::getTenantId();
        
//         if (tenantId_.empty())
//             throw std::runtime_error("Tenant ID not set in RequestContext");
//     }

//     /**
//      * @brief 查詢所有記錄（自動添加 tenant_id 條件）
//      */
//     std::vector<T> findAll()
//     {
//         return mapper_.findBy(
//             Criteria(T::Cols::_tenant_id, CompareOperator::EQ, tenantId_)
//         );
//     }

//     /**
//      * @brief 根據條件查詢（自動添加 tenant_id 條件）
//      */
//     std::vector<T> findBy(const Criteria& criteria)
//     {
//         return mapper_.findBy(
//             criteria && Criteria(T::Cols::_tenant_id, CompareOperator::EQ, tenantId_)
//         );
//     }

//     /**
//      * @brief 查詢單條記錄（自動添加 tenant_id 條件）
//      */
//     T findOne(const Criteria& criteria)
//     {
//         return mapper_.findOne(
//             criteria && Criteria(T::Cols::_tenant_id, CompareOperator::EQ, tenantId_)
//         );
//     }

//     /**
//      * @brief 通過主鍵查詢（自動驗證 tenant_id）
//      */
//     T findById(const std::string& id)
//     {
//         auto result = mapper_.findBy(
//             Criteria(T::Cols::_id, CompareOperator::EQ, id) &&
//             Criteria(T::Cols::_tenant_id, CompareOperator::EQ, tenantId_)
//         );

//         if (result.empty())
//             throw UnexpectedRows("Record not found or access denied");

//         return result.front();
//     }

//     /**
//      * @brief 計數（自動添加 tenant_id 條件）
//      */
//     size_t count(const Criteria& criteria = Criteria())
//     {
//         if (criteria.isNull())
//             return mapper_.count(Criteria(T::Cols::_tenant_id, CompareOperator::EQ, tenantId_));
        
//         return mapper_.count(
//             criteria && Criteria(T::Cols::_tenant_id, CompareOperator::EQ, tenantId_)
//         );
//     }

//     /**
//      * @brief 插入記錄（自動設置 tenant_id）
//      */
//     void insert(T& obj)
//     {
//         obj.setTenantId(tenantId_);
//         mapper_.insert(obj);
//     }

//     /**
//      * @brief 異步插入
//      */
//     void insert(T obj,
//                 const typename Mapper<T>::SingleRowCallback& rcb,
//                 const ExceptionCallback& ecb)
//     {
//         obj.setTenantId(tenantId_);
//         mapper_.insert(obj, rcb, ecb);
//     }

//     /**
//      * @brief 更新記錄（驗證 tenant_id）
//      */
//     size_t update(const T& obj)
//     {
//         if (obj.getValueOfTenantId() != tenantId_)
//             throw std::runtime_error("Cannot update record from different tenant");
        
//         return mapper_.update(obj);
//     }

//     /**
//      * @brief 刪除記錄（驗證 tenant_id）
//      */
//     size_t deleteOne(const T& obj)
//     {
//         if (obj.getValueOfTenantId() != tenantId_)
//             throw std::runtime_error("Cannot delete record from different tenant");
        
//         return mapper_.deleteOne(obj);
//     }

//     /**
//      * @brief 根據條件刪除（自動添加 tenant_id 條件）
//      */
//     size_t deleteBy(const Criteria& criteria)
//     {
//         return mapper_.deleteBy(
//             criteria && Criteria(T::Cols::_tenant_id, CompareOperator::EQ, tenantId_)
//         );
//     }

//     /**
//      * @brief 通過主鍵刪除（自動驗證 tenant_id）
//      */
//     size_t deleteById(const std::string& id)
//     {
//         return deleteBy(Criteria(T::Cols::_id, CompareOperator::EQ, id));
//     }

//     /**
//      * @brief 異步查詢
//      */
//     void findBy(const Criteria& criteria,
//                 const typename Mapper<T>::MultipleRowsCallback& rcb,
//                 const ExceptionCallback& ecb)
//     {
//         mapper_.findBy(
//             criteria && Criteria(T::Cols::_tenant_id, CompareOperator::EQ, tenantId_),
//             rcb, ecb
//         );
//     }

//     /**
//      * @brief 異步查詢所有
//      */
//     void findAll(const typename Mapper<T>::MultipleRowsCallback& rcb,
//                  const ExceptionCallback& ecb)
//     {
//         findBy(Criteria(), rcb, ecb);
//     }

//     /**
//      * @brief Future 方式查詢
//      */
//     std::future<std::vector<T>> findFutureBy(const Criteria& criteria)
//     {
//         return mapper_.findFutureBy(
//             criteria && Criteria(T::Cols::_tenant_id, CompareOperator::EQ, tenantId_)
//         );
//     }

//     /**
//      * @brief 獲取底層 Mapper
//      */
//     Mapper<T>& getMapper()
//     {
//         return mapper_;
//     }

//     /**
//      * @brief 支持鏈式調用 - limit
//      */
//     TenantRepository<T>& limit(size_t limit)
//     {
//         mapper_.limit(limit);
//         return *this;
//     }

//     /**
//      * @brief 支持鏈式調用 - offset
//      */
//     TenantRepository<T>& offset(size_t offset)
//     {
//         mapper_.offset(offset);
//         return *this;
//     }

//     /**
//      * @brief 支持鏈式調用 - orderBy
//      */
//     TenantRepository<T>& orderBy(const std::string& colName, 
//                                  const SortOrder& order = SortOrder::ASC)
//     {
//         mapper_.orderBy(colName, order);
//         return *this;
//     }

//     /**
//      * @brief 支持鏈式調用 - paginate
//      */
//     TenantRepository<T>& paginate(size_t page, size_t perPage)
//     {
//         mapper_.paginate(page, perPage);
//         return *this;
//     }

//     /**
//      * @brief 獲取當前租戶 ID
//      */
//     const std::string& getTenantId() const
//     {
//         return tenantId_;
//     }

// private:
//     Mapper<T> mapper_;
//     std::string tenantId_;
// };
