#pragma once

#include <regex>
#include <string>

inline bool isValidUuid(const std::string &value)
{
    static const std::regex uuidPattern(
        "^[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[1-5][0-9a-fA-F]{3}-[89aAbB][0-9a-fA-F]{3}-[0-9a-fA-F]{12}$");
    return std::regex_match(value, uuidPattern);
}
