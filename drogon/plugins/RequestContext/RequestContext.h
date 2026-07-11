#pragma once
#include <string>
#include <drogon/drogon.h>
class RequestContext
{
public:
    static void setTenantId(const std::string &id)
    {
        tenantId = id;
    }
    static std::string getTenantId()
    {
        return tenantId;
    }
    static std::string getTeamId()
    {
        return teamId;
    }
    static void setUserId(const std::string &id)
    {
        LOG_DEBUG << "Setting userId in RequestContext: " << id;
        userId = id;
    }
    static void setTeamId(const std::string &id)
    {
        LOG_DEBUG << "Setting teamId in RequestContext: " << id;
        teamId = id;
    }
    static std::string getUserId()
    {
        return userId;
    }

private:
    static thread_local std::string tenantId;
    static thread_local std::string teamId;
    static thread_local std::string userId;     
    // 其他需要在 request 中共享的資料
};

// thread_local std::string RequestContext::tenantId = "";