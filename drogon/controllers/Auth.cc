#include "Auth.h"
#include <jsoncpp/json/json.h>
#include <drogon/HttpResponse.h>
#include "models/Users.h"
#include "models/Tenants.h"
#include "utils/PasswordUtil.h"
#include "utils/mapToJson.h"
#include <argon2.h>
#include <drogon/drogon.h>
#include <chrono>
#include <cassert>
#include <iostream>
#include "jwt/jwt.hpp"
#include "models/Teams.h"
#include "utils/readSecretFromFile.h"
#include "Config.h"
using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::teams_manager;

void Auth::registerUser(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback)
{
    auto json = req->getJsonObject();
    DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
    orm::Mapper<Tenants> tenantMapper(clientPtr);
    orm::Mapper<Users> userMapper(clientPtr);
    orm::Mapper<Teams> teamMapper(clientPtr);
    if (!json)
    {
        callback(HttpResponse::newHttpResponse());
        return;
    }
    // LOG_DEBUG << "Received JSON: " << (*json).toStyledString();
    std::string tenant_name = (*json)["tenant_name"].asString();
    std::string username = (*json)["username"].asString();
    std::string email = (*json)["email"].asString();
    std::string password = (*json)["password"].asString();
    std::string team_id = (*json)["team_id"].asString();
    
    
    try
    {
        Tenants tenant = tenantMapper.findOne(
            Criteria(
                Tenants::Cols::_name,
                CompareOperator::EQ,
                tenant_name));

        Teams team = teamMapper.findByPrimaryKey(team_id);
        Users user;
        user.setUsername(username);
        user.setEmail(email);
        user.setPassword(PasswordUtil::hash(password));
        user.setTenantId(tenant.getValueOfId());
        user.setTenantName(tenant.getValueOfName());
        user.setTeamId(team_id);

        user.setTeamName(team.getValueOfName());
        userMapper.insert(user);

        Json::Value response;
        Json::Value data = user.toJson();
        data.removeMember(Users::Cols::_password);
        response["data"] = data;
        auto resp = HttpResponse::newHttpJsonResponse(response);
        callback(resp);
    }
    catch (const std::exception &e)
    {
        Json::Value errorResponse;
        errorResponse["error"] = e.what();

        auto resp = HttpResponse::newHttpJsonResponse(errorResponse);
        resp->setStatusCode(k400BadRequest);
        callback(resp);
    }
}

void Auth::loginUser(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback)
{
    std::shared_ptr<Json::Value> json = req->getJsonObject();
    DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager");
    orm::Mapper<Tenants> tenantMapper(clientPtr);
    orm::Mapper<Users> userMapper(clientPtr);
    if (!json)
    {
        callback(HttpResponse::newHttpResponse());
        return;
    }

    std::string tenant_name = (*json)["tenant_name"].asString();
    std::string username = (*json)["username"].asString();
    std::string password = (*json)["password"].asString();

    try
    {
        std::string tenant_id = tenantMapper.findOne(
                                                Criteria(
                                                    Tenants::Cols::_name,
                                                    CompareOperator::EQ,
                                                    tenant_name))
                                    .getValueOfId();

        Users user = userMapper.findOne(
            Criteria(
                Users::Cols::_username,
                CompareOperator::EQ,
                username) &&
            Criteria(
                Users::Cols::_tenant_id,
                CompareOperator::EQ,
                tenant_id));

        if (PasswordUtil::verify(password,
                                 user.getValueOfPassword()))
        {
            // static std::string key = readSecretFromFile("../drogon/.secret");
            std::string key = Config::jwtSecret;
            // auto key = "secret"; // Secret to use for the algorithm
            // Create JWT object
            jwt::jwt_object obj{
                jwt::params::algorithm("HS256"),
                jwt::params::payload({
                    {"user_id", user.getValueOfId()},
                    {"user_name", user.getValueOfUsername()},
                    {"username", user.getValueOfUsername()},
                    {"tenant_id", tenant_id},
                    {"tenant_name", tenant_name},
                    {"team_id", user.getValueOfTeamId()},
                    {"team_name", user.getValueOfTeamName()}
                }),
                jwt::params::secret(key)};

            obj.add_claim("exp", std::chrono::system_clock::now() + std::chrono::seconds(7200));
            // Get the encoded string/assertion
            auto jwt_str = obj.signature();
            LOG_DEBUG << "JWT: " << jwt_str;
            // 解碼並驗證
            // auto decoded_obj = jwt::decode(jwt_str, jwt::params::algorithms({"HS256"}), jwt::params::secret(key));

            // std::cout << "Header: " << decoded_obj.header() << std::endl;
            // std::cout << "Payload: " << decoded_obj.payload() << std::endl;
            // Decode
            // auto dec_obj = jwt::decode(enc_str, algorithms({"HS256"}), secret(key));
            // std::cout << dec_obj.header() << std::endl;
            // std::cout << dec_obj.payload() << std::endl;

            Json::Value response;
            response["user_id"] = user.getValueOfId();
            response["user_name"] = user.getValueOfUsername();
            response["tenant_id"] = tenant_id;
            response["tenant_name"] = tenant_name;
            response["team_id"] = user.getValueOfTeamId();
            response["team_name"] = user.getValueOfTeamName();
            response["message"] = "Login success";
            response["token"] = jwt_str;
            callback(HttpResponse::newHttpJsonResponse(response));
        }
        else
        {
            drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(Json::Value("Wrong password"));
            resp->setStatusCode(k400BadRequest);
            callback(resp);
        }
    }
    catch (const drogon::orm::UnexpectedRows &e)
    {

        Json::Value errorResponse;
        errorResponse["error"] = e.what();

        auto resp = HttpResponse::newHttpJsonResponse(errorResponse);
        resp->setStatusCode(k400BadRequest);

        callback(resp);
    }
}