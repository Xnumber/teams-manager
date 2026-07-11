#include "RequestContext.h"

thread_local std::string RequestContext::tenantId = "";
thread_local std::string RequestContext::userId = "";
thread_local std::string RequestContext::teamId = "";