#pragma once
#include "RequestContext.h"

class BaseEntity
{
public:
    void applyTenantId()
    {
        setTenantId(RequestContext::getTenantId());
    }
    protected:
    virtual void setTenantId(const std::string &tenantId) = 0;
};