/**
 *
 *  AuthFilter.cc
 *
 */
#include "../plugins/RequestContext/RequestContext.h"
#include "AuthFilter.h"
#include "jwt/jwt.hpp"


#include "utils/readSecretFromFile.h"
#include "Config.h"
using namespace drogon;

void AuthFilter::doFilter(const HttpRequestPtr &req,
                          FilterCallback &&fcb,
                          FilterChainCallback &&fccb)
{
    // //Edit your logic here
    // if (1)
    // {
    //     //Passed
    //     fccb();
    //     return;
    // }
    // //Check failed
    // auto res = drogon::HttpResponse::newHttpResponse();
    // res->setStatusCode(k500InternalServerError);
    // fcb(res);



    // Skip the verification on method Options
    if (req->getMethod() == HttpMethod::Options)
        return fccb();

    const std::string &token = req->getHeader("Authorization");
    LOG_DEBUG << "Authorization header: " << token;
    // If the authorization header is empty or if the length is lower than 7 characters, means "Bearer " is not included on authorization header string.
    if (token.length() < 7)
    {
        Json::Value resultJson;
        resultJson["error"] = "No header authentication!";
        // resultJson["status"] = 0;

        auto res = HttpResponse::newHttpJsonResponse(resultJson);
        res->setStatusCode(k401Unauthorized);

        // Return the response and let's tell this endpoint request was cancelled
        return fcb(res);
    }

    try
    {
        // 解碼並驗證
        // static std::string key = readSecretFromFile("../drogon/.secret");
        std::string key = Config::jwtSecret;
        std::string tokenStr = token.substr(7);
        LOG_DEBUG << "JWT Token: " << tokenStr;
        jwt::jwt_object jwtAttributes = jwt::decode(tokenStr, jwt::params::algorithms({"HS256"}), jwt::params::secret(key));
        // LOG_DEBUG << "JWT Payload: " << jwtAttributes.payload();
        // Log each claim in the payload

        // std::cout << "JWT Claims:" << jwtAttributes.payload() << std::endl;
        jwt::jwt_payload payload = jwtAttributes.payload();
        std::string tenant_id = payload.get_claim_value<std::string>("tenant_id");
        std::string user_id = payload.get_claim_value<std::string>("user_id");
        std::string team_id = payload.get_claim_value<std::string>("team_id");
        // std::cout << "Extracted tenant_id from JWT: " << tenant_id << std::endl;
        // std::cout << "Extracted user_id from JWT: " << user_id << std::endl;
        // std::cout << "Extracted team_id from JWT: " << team_id << std::endl;

        req->getAttributes()->insert("tenant_id", tenant_id);
        req->getAttributes()->insert("user_id", user_id);
        req->getAttributes()->insert("jwt_payload", payload);
        RequestContext::setTenantId(tenant_id);
        RequestContext::setUserId(user_id);

        RequestContext::setTeamId(team_id);
        // for (auto& attribute : jwtAttributes)
        //     request->getAttributes()->insert("jwt_" + attribute.first, attribute.second);
        return fccb();
    }
    catch (const std::exception &e)
    {
        Json::Value resultJson;
        resultJson["error"] = "Token is invalid!";
        // resultJson["status"] = 0;

        // res.setHeader('Access-Control-Allow-Origin', '*');
        // res.setHeader('Access-Control-Allow-Credentials', 'true');

        
        auto res = HttpResponse::newHttpJsonResponse(resultJson);
        res->addHeader("Access-Control-Allow-Origin", "*");
        res->addHeader("Access-Control-Allow-Credentials", "true");
        res->setStatusCode(k401Unauthorized);
        
        std::cerr << e.what() << '\n';
        return fcb(res);
    }

    // if (jwtAttributes.empty()) {
    //     Json::Value resultJson;
    //     resultJson["error"] = "Token is invalid!";
    //     resultJson["status"] = 0;

    //     auto res = HttpResponse::newHttpJsonResponse(resultJson);
    //     res->setStatusCode(k401Unauthorized);

    //     return fcb(res);
    // }

    // Save the claims on attributes, for on next endpoint to be accessible
    // for (auto& attribute : jwtAttributes)
    //     request->getAttributes()->insert("jwt_" + attribute.first, attribute.second);

    // If everything is right, just move to other endpoint
    // return fccb();
}
