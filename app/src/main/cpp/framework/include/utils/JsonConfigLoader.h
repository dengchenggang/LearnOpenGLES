#ifndef C_JSON_CONFIG_LOADER_H
#define C_JSON_CONFIG_LOADER_H

#include <nlohmann/json.hpp>
#include <string>
#include <map>
#include <vector>
#include <shared_mutex>

class JsonConfigLoader {
public:
    JsonConfigLoader() = default;
    ~JsonConfigLoader() = default;
    JsonConfigLoader(const JsonConfigLoader&) = delete;
    JsonConfigLoader& operator=(const JsonConfigLoader&) = delete;
public:
    bool parse(const std::string& jsonString);

    bool has(const std::string& key) const;

    template<typename T>
    void set(const std::string& key, const T& value) {
        std::unique_lock<std::shared_mutex> lock(mMutex);
        mCache[key] = value;
    }

    template<typename T>
    T get(const std::string& key, const T& defaultValue) const {
        std::shared_lock<std::shared_mutex> lock(mMutex);

        auto cacheIt = mCache.find(key);
        if (cacheIt != mCache.end()) {
            return cacheIt->second.get<T>();
        }

        auto parts = splitKey(key);
        if (!has(parts)) {
            return defaultValue;
        }

        auto result = mCache.emplace(key, getNode(parts));
        return result.first->second.get<T>();
    }

private:
    static std::vector<std::string> splitKey(const std::string& key);
    bool has(const std::vector<std::string>& parts) const;
    nlohmann::json::const_reference getNode(const std::vector<std::string>& parts) const;

private:
    nlohmann::json mJson;
    mutable std::map<std::string, nlohmann::json> mCache;
    mutable std::shared_mutex mMutex;
};

#endif
