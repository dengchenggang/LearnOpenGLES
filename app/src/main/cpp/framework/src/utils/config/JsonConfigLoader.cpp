#include "JsonConfigLoader.h"
#include "Log.h"
#include <sstream>

std::vector<std::string> JsonConfigLoader::splitKey(const std::string& key) {
    std::vector<std::string> parts;
    std::stringstream ss(key);
    std::string part;
    while (std::getline(ss, part, '.')) {
        if (!part.empty()) {
            parts.push_back(part);
        }
    }
    return parts;
}

bool JsonConfigLoader::parse(const std::string& jsonString) {
    std::unique_lock<std::shared_mutex> lock(mMutex);
    try {
        mJson = nlohmann::json::parse(jsonString);
        mCache.clear();
    } catch (const std::exception& e) {
        LogE("failed to parse json string, error: %s", e.what());
        return false;
    }

    return true;
}

bool JsonConfigLoader::has(const std::string& key) const {
    std::shared_lock<std::shared_mutex> lock(mMutex);
    return has(splitKey(key));
}

bool JsonConfigLoader::has(const std::vector<std::string>& parts) const {
    if (parts.empty()) {
        return false;
    }

    const nlohmann::json* node = &mJson;
    for (const auto& part : parts) {
        if (!node->is_object() || node->find(part) == node->end()) {
            return false;
        }
        node = &(*node)[part];
    }

    return true;
}

nlohmann::json::const_reference JsonConfigLoader::getNode(const std::vector<std::string>& parts) const {
    const nlohmann::json* node = &mJson;
    for (const auto& part : parts) {
        node = &(*node)[part];
    }
    return *node;
}
