#pragma once
#include <drogon/orm/Mapper.h>

template <typename T>
class ProjectMapper
{
    using Mapper = drogon::orm::Mapper<T>;
    using Criteria = drogon::orm::Criteria;
    using CompareOperator = drogon::orm::CompareOperator;

  public:
    ProjectMapper(drogon::orm::DbClientPtr client, int tenantId)
        : mapper_(client), tenantId_(tenantId)
    {}

    std::vector<T> findAll()
    {
        return mapper_.findBy(
            Criteria("tenant_id", CompareOperator::EQ, tenantId_)
        );
    }

  protected:
    Mapper mapper_;
    int tenantId_;
};