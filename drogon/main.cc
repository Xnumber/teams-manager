#include <drogon/drogon.h>
#include <drogon/orm/DbClient.h>
#include <drogon/orm/Mapper.h>
#include <iostream>
#include "hello.h"
#include "Config.h"
using namespace drogon;
using namespace drogon::orm;






int main()
{
    sayHello();


    
    drogon::app().loadConfigFile("config.json");
    // drogon::app().run();
    Config::jwtSecret = drogon::app()
        .getCustomConfig()["secret"]
        .asString();


    drogon::app().registerSyncAdvice([](const drogon::HttpRequestPtr &req) -> drogon::HttpResponsePtr
                                     {
			if(req->method() == drogon::HttpMethod::Options)
			{
				auto resp = drogon::HttpResponse::newHttpResponse();
				{
					const auto& val = req->getHeader("Origin");
					if(!val.empty())
						resp->addHeader("Access-Control-Allow-Origin", val);
				}
				{
					const auto& val = req->getHeader("Access-Control-Request-Method");
					if(!val.empty())
						resp->addHeader("Access-Control-Allow-Methods", val);
				}
				resp->addHeader("Access-Control-Allow-Credentials", "true");
				{
					const auto& val = req->getHeader("Access-Control-Request-Headers");
					if(!val.empty())
						resp->addHeader("Access-Control-Allow-Headers", val);
				}
				return std::move(resp);
			}
			return {}; })
        .registerPostHandlingAdvice([](const drogon::HttpRequestPtr &req, const drogon::HttpResponsePtr &resp) -> void
                                    {
			{
				const auto& val = req->getHeader("Origin");
				if(!val.empty())
					resp->addHeader("Access-Control-Allow-Origin", val);
			}
			{
				const auto& val = req->getHeader("Access-Control-Request-Method");
				if(!val.empty())
					resp->addHeader("Access-Control-Allow-Methods", val);
			}
			resp->addHeader("Access-Control-Allow-Credentials", "true");
			{
				const auto& val = req->getHeader("Access-Control-Request-Headers");
				if(!val.empty())
					resp->addHeader("Access-Control-Allow-Headers", val);
			} })
        .run();
    
    return 0;
}
