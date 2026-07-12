#pragma once
#include <optional>
#include <string>
#include <memory>
#include <jsoncpp/json/json.h>

inline std::optional<std::string> optStr(const Json::Value &json, const char *k)
{
    if (!json.isMember(k))
        return std::nullopt;
    std::string v = json[k].asString();
    if (v.empty())
        return std::nullopt;
    return v;
}

inline std::optional<std::string> optStr(const std::shared_ptr<Json::Value> &json, const char *k)
{
    if (!json)
        return std::nullopt;
    return optStr(*json, k);
}
