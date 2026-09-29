#include "AssetManager.h"
#include "utils/Log.h"

#include <android/hardware_buffer.h>

#undef AssetManager

KEY_VALUE(TAG, AssetManager)

namespace engine {
namespace renderer {

AssetManager::~AssetManager() = default;

TextureHardwareBufferPtr AssetManager::loadTexture(const std::string& url, AHardwareBuffer* buffer) {
    if (!buffer) {
        LogW("%s buffer is null", TAG);
        return nullptr;
    }

    auto key = reinterpret_cast<uintptr_t>(buffer);
    auto& inner = mTextureHardwareBufferCache[url];  // 不存在则自动创建空 map

    auto it = inner.find(key);
    if (it != inner.end()) {
        LogD("%s cache hit: url=%s, buffer=%p, tex=%u, refs=%ld",
             TAG, url.c_str(), buffer, it->second->getHandle(), it->second.use_count());
        return it->second;  // shared_ptr 副本，refcount+1
    }

    // 未命中：新建并缓存
    auto tex = std::make_shared<TextureHardwareBuffer>();
    if (!tex->create(buffer)) {
        LogE("%s failed to create TextureHardwareBuffer: url=%s, buffer=%p",
             TAG, url.c_str(), buffer);
        return nullptr;
    }

    inner[key] = tex;

    LogD("%s new texture: url=%s, buffer=%p, tex=%u, size=%dx%d",
         TAG, url.c_str(), buffer, tex->getHandle(), tex->getWidth(), tex->getHeight());
    return tex;
}

void AssetManager::unloadTexture(const std::string& url) {
    auto it = mTextureHardwareBufferCache.find(url);
    if (it == mTextureHardwareBufferCache.end()) return;

    size_t count = it->second.size();
    it->second.clear();
    LogD("%s unload: url=%s, removed=%zu entries", TAG, url.c_str(), count);
}

void AssetManager::releaseAll() {
    size_t total = 0;
    for (auto& [url, inner] : mTextureHardwareBufferCache) {
        total += inner.size();
    }
    mTextureHardwareBufferCache.clear();
    LogD("%s releaseAll: removed=%zu entries", TAG, total);
}

} // namespace renderer
} // namespace engine