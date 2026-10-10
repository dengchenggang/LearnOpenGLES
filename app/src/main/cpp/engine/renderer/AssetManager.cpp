#include "AssetManager.h"
#include "texture/Texture2D.h"
#include "filesystem/FileSystem.h"
#include "filesystem/ImageData.h"
#include "utils/Log.h"

#include <android/hardware_buffer.h>

#undef AssetManager

KEY_VALUE(TAG, AssetManager)

namespace engine {
namespace renderer {

AssetManager::~AssetManager() = default;

TexturePtr AssetManager::loadTexture(const std::string& path) {
    if (path.empty()) {
        LogW("%s path is empty", TAG);
        return nullptr;
    }

    auto it = mTextureCache.find(path);
    if (it != mTextureCache.end()) {
        LogD("%s cache hit: path=%s, tex=%u, refs=%ld",
             TAG, path.c_str(), it->second->getHandle(), it->second.use_count());
        return it->second;
    }

    auto fileData = FileSystem.readFile(path.c_str());
    if (!fileData || fileData->empty()) {
        LogE("%s failed to read file: path=%s", TAG, path.c_str());
        return nullptr;
    }

    auto* imageData = dynamic_cast<framework::ImageData*>(fileData.get());
    if (!imageData) {
        LogE("%s not an image file: path=%s", TAG, path.c_str());
        return nullptr;
    }

    TextureFormat format;
    switch (imageData->format) {
        case framework::EImageDataFormat::RGBA_8888:
            format = TextureFormat::RGBA8;
            break;
        case framework::EImageDataFormat::RGB_888:
            format = TextureFormat::RGB8;
            break;
        default:
            LogE("%s unsupported image format: path=%s", TAG, path.c_str());
            return nullptr;
    }

    auto tex = std::make_shared<Texture2D>();
    if (!tex->create(imageData->width, imageData->height, format, imageData->data())) {
        LogE("%s failed to create Texture2D: path=%s, size=%dx%d",
             TAG, path.c_str(), imageData->width, imageData->height);
        return nullptr;
    }

    mTextureCache[path] = tex;

    LogD("%s new texture: path=%s, tex=%u, size=%dx%d",
         TAG, path.c_str(), tex->getHandle(), tex->getWidth(), tex->getHeight());
    return tex;
}

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

void AssetManager::unloadTexture(const std::string& key) {
    auto texIt = mTextureCache.find(key);
    if (texIt != mTextureCache.end()) {
        mTextureCache.erase(texIt);
        LogD("%s unload texture2D: path=%s", TAG, key.c_str());
    }

    auto it = mTextureHardwareBufferCache.find(key);
    if (it != mTextureHardwareBufferCache.end()) {
        size_t count = it->second.size();
        it->second.clear();
        LogD("%s unload hwbuffer: url=%s, removed=%zu entries", TAG, key.c_str(), count);
    }
}

void AssetManager::releaseAll() {
    size_t total = mTextureCache.size();
    for (auto& [url, inner] : mTextureHardwareBufferCache) {
        total += inner.size();
    }
    mTextureCache.clear();
    mTextureHardwareBufferCache.clear();
    LogD("%s releaseAll: removed=%zu entries", TAG, total);
}

} // namespace renderer
} // namespace engine