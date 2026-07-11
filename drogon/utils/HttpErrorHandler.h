#pragma once
#include <drogon/HttpResponse.h>

drogon::HttpResponsePtr makeErrorResponse(drogon::HttpStatusCode code, const std::string &message);