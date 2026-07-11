#include "HttpErrorHandler.h"
#include <jsoncpp/json/json.h>

drogon::HttpResponsePtr makeErrorResponse(drogon::HttpStatusCode code, const std::string &message)
{
    Json::Value err;
    err["result"] = "error";
    err["message"] = message;
    auto resp = drogon::HttpResponse::newHttpJsonResponse(err);
    resp->setStatusCode(code);
    return resp;
}