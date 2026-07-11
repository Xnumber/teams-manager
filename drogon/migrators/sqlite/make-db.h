#pragma once
#include <drogon/orm/DbClient.h>
#include <optional>

std::optional<drogon::orm::DbClientPtr> makeDb();   // 函式宣告