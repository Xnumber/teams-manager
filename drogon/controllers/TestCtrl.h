#pragma once

#include <drogon/HttpSimpleController.h>

using namespace drogon;

class TestCtrl : public drogon::HttpSimpleController<TestCtrl>
{
  public:
    void asyncHandleHttpRequest(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback) override;
    PATH_LIST_BEGIN
    // list path definitions here;
    // PATH_ADD("/path", "filter1", "filter2", HttpMethod1, HttpMethod2...);
    PATH_ADD("/",Get);
    PATH_ADD("/get",Get);
    PATH_ADD("/post",Post);
    PATH_ADD("/put",Put);
    PATH_ADD("/update/{a}", Put);
    PATH_ADD("/delete",Delete);

    PATH_LIST_END
};