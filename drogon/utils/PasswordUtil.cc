#include "PasswordUtil.h"
#include <argon2.h>
#include <random>
#include <stdexcept>

namespace
{
std::string generateSalt(size_t length = 16)
{
    static const char charset[] =
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "0123456789";

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, sizeof(charset) - 2);

    std::string salt;
    for (size_t i = 0; i < length; ++i)
        salt += charset[dist(gen)];

    return salt;
}
}

std::string PasswordUtil::hash(const std::string& password)
{
    std::string salt = generateSalt();

    char encoded[256];

    int result = argon2id_hash_encoded(
        2,              // iterations
        1 << 16,        // 64 MB memory
        1,              // parallelism
        password.c_str(),
        password.size(),
        salt.c_str(),
        salt.size(),
        32,             // hash length
        encoded,
        sizeof(encoded)
    );

    if (result != ARGON2_OK)
        throw std::runtime_error("Argon2 hash failed");

    return std::string(encoded);
}

bool PasswordUtil::verify(const std::string& password,
                          const std::string& storedHash)
{
    return argon2id_verify(
        storedHash.c_str(),
        password.c_str(),
        password.size()
    ) == ARGON2_OK;
}