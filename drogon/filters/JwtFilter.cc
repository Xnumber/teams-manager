#include "JwtFilter.h"
#include "jwt/jwt.hpp"
using namespace api::v1::filters;

void JwtFilter::doFilter(const HttpRequestPtr &request, FilterCallback &&fcb, FilterChainCallback &&fccb)
{
    // Skip the verification on method Options
    if (request->getMethod() == HttpMethod::Options)
        return fccb();

    const std::string &token = request->getHeader("Authorization");

    // If the authorization header is empty or if the length is lower than 7 characters, means "Bearer " is not included on authorization header string.
    if (token.length() < 7)
    {
        Json::Value resultJson;
        resultJson["error"] = "No header authentication!";
        resultJson["status"] = 0;

        auto res = HttpResponse::newHttpJsonResponse(resultJson);
        res->setStatusCode(k401Unauthorized);

        // Return the response and let's tell this endpoint request was cancelled
        return fcb(res);
    }

    // Remove the string "Bearer " on token and decode it
    // std::map<std::string, any> jwtAttributes = JWT::decodeToken(token.substr(7));

    // 解碼並驗證
    // auto key = "secret";
    // auto jwtAttributes = jwt::decode(token.substr(7), jwt::params::algorithms({"HS256"}), jwt::params::secret(key));

    // std::cout << "Header: " << decoded_obj.header() << std::endl;
    // std::cout << "Payload: " << decoded_obj.payload() << std::endl;
    // Decode
    // auto dec_obj = jwt::decode(enc_str, algorithms({"HS256"}), secret(key));
    // std::cout << dec_obj.header() << std::endl;
    // std::cout << dec_obj.payload() << std::endl;

    try
    {

        // 解碼並驗證
        auto key = "secret";
        std::string tokenStr = token.substr(7);
        LOG_DEBUG << "JWT Token: " << tokenStr;
        jwt::jwt_object jwtAttributes = jwt::decode(tokenStr, jwt::params::algorithms({"HS256"}), jwt::params::secret(key));
        // LOG_DEBUG << "JWT Payload: " << jwtAttributes.payload();
        // Log each claim in the payload

        // std::cout << "JWT Claims:" << jwtAttributes.payload() << std::endl;

        /* code */
        Json::Value resultJson;
        resultJson["error"] = "Token is invalid!";
        resultJson["status"] = 0;

        auto res = HttpResponse::newHttpJsonResponse(resultJson);
        // res->setStatusCode(k401Unauthorized);

        // for (auto& attribute : jwtAttributes)
        //     request->getAttributes()->insert("jwt_" + attribute.first, attribute.second);
        return fccb();
    }
    catch (const std::exception &e)
    {
        Json::Value resultJson;
        resultJson["error"] = "Token is invalid!";
        resultJson["status"] = 0;

        auto res = HttpResponse::newHttpJsonResponse(resultJson);
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