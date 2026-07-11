// https://drogonframework.github.io/drogon-docs/#/ENG/ENG-04-2-Controller-HttpController?id=path-mapping
#include "TestCtrl.h"
#include <json/json.h>
#include <drogon/drogon.h>

using namespace drogon::orm;
void TestCtrl::asyncHandleHttpRequest(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(k200OK);
    resp->setContentTypeCode(CT_TEXT_HTML);
    if (req->path() == "/" && req->method() == Get)
    {
        resp->setBody("Hello, this is the root path.");
    }
    if (req->path() == "/get" && req->method() == Get)
    {
        DbClientPtr clientPtr = drogon::app().getDbClient("teams_manager"); 

        // auto result = clientPtr2->execSqlSync("SELECT * FROM users");
        // std::cout << "\n查询结果：" << std::endl;
        // std::cout << "共 " << result.size() << " 条记录" << std::endl;

        // for (auto row : result)
        // {
        //     std::cout << "ID: " << row["id"].as<int>()
        //               << ", 姓名: " << row["username"].as<std::string>()
        //               << ", 邮箱: " << row["email"].as<std::string>()
        //               << ", 年龄: " << row["age"].as<int>()
        //               << std::endl;
        // }

        // 解析 query parameters
        auto queryParams = req->getParameters();

        // 取得特定的 query parameter
        std::string paramValue = req->getParameter("paramName");

        // 解析 headers
        auto headers = req->getHeaders();

        // 取得特定的 header
        std::string userAgent = req->getHeader("User-Agent");
        std::string contentType = req->getHeader("Content-Type");

        // 建立回應內容
        Json::Value jsonResponse;
        jsonResponse["message"] = "GET request received.";

        // 將 query parameters 加入回應
        Json::Value queryJson;
        for (const auto &param : queryParams)
        {
            queryJson[param.first] = param.second;
        }
        jsonResponse["queryParams"] = queryJson;

        // 將 headers 加入回應
        Json::Value headersJson;
        for (const auto &header : headers)
        {
            headersJson[header.first] = header.second;
        }
        jsonResponse["headers"] = headersJson;

        resp->setContentTypeCode(CT_APPLICATION_JSON);
        resp->setBody(jsonResponse.toStyledString());
    }
    else if (req->path() == "/post" && req->method() == Post)
    {
        // 取得原始 body 字串
        std::string_view rawBody = req->body();
        //  // 嘗試解析 JSON body
        auto jsonPtr = req->getJsonObject();

        Json::Value jsonResponse;
        // fetch('/post', {
        //     method: 'POST',
        //     headers: { 'Content-Type': 'application/json' },
        //     body: JSON.stringify({ qqq: 1 })
        // })
        if (jsonPtr)
        {
            jsonResponse["message"] = "POST request received.";
            jsonResponse["body"] = *jsonPtr;
        }
        else
        {
            jsonResponse["message"] = "POST request received, but body is not valid JSON.";
            jsonResponse["rawBody"] = std::string(rawBody);
        }

        resp->setContentTypeCode(CT_APPLICATION_JSON);

        resp->setBody(jsonResponse.toStyledString());
    }
    else if (req->path() == "/put" && req->method() == Put)
    {
        std::string id = req->getParameter("1");
        // 取得原始 body 字串
        std::string_view rawBody = req->body();
        //  // 嘗試解析 JSON body
        auto jsonPtr = req->getJsonObject();

        Json::Value jsonResponse;

        // fetch('/post', {
        //     method: 'POST',
        //     headers: { 'Content-Type': 'application/json' },
        //     body: JSON.stringify({ qqq: 1 })
        // })
        if (jsonPtr)
        {
            jsonResponse["message"] = "PUT request received.";
            jsonResponse["body"] = *jsonPtr;
            jsonResponse["id"] = id;
        }
        else
        {
            jsonResponse["message"] = "PUT request received, but body is not valid JSON.";
            jsonResponse["rawBody"] = std::string(rawBody);
        }

        resp->setContentTypeCode(CT_APPLICATION_JSON);

        resp->setBody(jsonResponse.toStyledString());
    }
    else if (req->method() == Put && req->matchedPathPattern() == "/update/{a}")
    {
        std::string id = req->getParameter("1");
        // 取得原始 body 字串
        std::string_view rawBody = req->body();
        //  // 嘗試解析 JSON body
        auto jsonPtr = req->getJsonObject();

        Json::Value jsonResponse;

        // fetch('/post', {
        //     method: 'POST',
        //     headers: { 'Content-Type': 'application/json' },
        //     body: JSON.stringify({ qqq: 1 })
        // })
        if (jsonPtr)
        {
            jsonResponse["message"] = "PUT request received.";
            jsonResponse["body"] = *jsonPtr;
            jsonResponse["id"] = id;
        }
        else
        {
            jsonResponse["message"] = "PUT request received, but body is not valid JSON.";
            jsonResponse["rawBody"] = std::string(rawBody);
        }

        resp->setContentTypeCode(CT_APPLICATION_JSON);

        resp->setBody(jsonResponse.toStyledString());
    }
    else if (req->path() == "/delete" && req->method() == Delete)
    {
        resp->setBody("DELETE request received.");
    }

    callback(resp);
}