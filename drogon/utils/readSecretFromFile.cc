#include <fstream>

std::string readSecretFromFile(const std::string& path) {
    std::ifstream file(path);
    std::string secret;
    if (file) {
        std::getline(file, secret);
    }
    return secret;
}