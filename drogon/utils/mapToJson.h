#pragma once
#include <jsoncpp/json/json.h>

// 輔助函數：類似 JS 的 array.map()
template <typename Container, typename Func>
Json::Value mapToJson(const Container &container, Func func)
{
    Json::Value result(Json::arrayValue);
    for (const auto &item : container)
    {
        result.append(func(item));
    }
    return result;
}
