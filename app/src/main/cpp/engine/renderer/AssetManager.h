#ifndef LEARNOPENGLES_ASSETMANAGER_H
#define LEARNOPENGLES_ASSETMANAGER_H

#include "texture/TextureHardwareBuffer.h"
#include "common/Singleton.hpp"
#include <unordered_map>
#include <memory>
#include <cstdint>
#include <string>

typedef struct AHardwareBuffer AHardwareBuffer;

namespace engine {
namespace renderer {

/// AssetManager：资源加载与缓存（单例）
///
/// 职责：
/// - Texture2D 缓存：key 为文件路径，自动通过 FileSystem 读取并解码
/// - 二级缓存 TextureHardwareBuffer：第一级 key 为 url（相机流），第二级 key 为 buffer 指针
/// - loadTexture 返回 shared_ptr，cache 和调用方各自持有一份引用
/// - unloadTexture(key) 清空指定 key，但已有 shared_ptr 持有者不受影响
///
/// 单例宏用法：
/// @code
///   auto tex = AssetManager.loadTexture("image/bg.png");
///   auto hwTex = AssetManager.loadTexture("camera://20", buffer);
///   AssetManager.unloadTexture("image/bg.png");
/// @endcode
class AssetManager {
    friend class framework::Singleton<AssetManager>;
public:
    AssetManager(const AssetManager&) = delete;
    AssetManager& operator=(const AssetManager&) = delete;

    // ---- Texture2D 缓存（文件路径） ----

    /// 从文件路径加载 Texture2D（shared_ptr）
    /// 缓存命中返回副本（refcount+1），未命中通过 FileSystem 读取解码并缓存。
    TexturePtr loadTexture(const std::string& path);

    // ---- TextureHardwareBuffer 缓存（相机流） ----

    /// 加载指定 url 下 buffer 对应的纹理（shared_ptr）
    /// 缓存命中返回副本（refcount+1），未命中新建并缓存。
    TextureHardwareBufferPtr loadTexture(const std::string& url, AHardwareBuffer* buffer);

    /// 清空指定 key 的全部纹理缓存（已有 shared_ptr 持有者不受影响）
    void unloadTexture(const std::string& key);

    /// 清空全部缓存
    void releaseAll();

private:
    AssetManager() = default;
    ~AssetManager();

    // Texture2D 缓存：path → Texture shared_ptr
    std::unordered_map<std::string, TexturePtr> mTextureCache;

    // 二级缓存：url → (buffer_pointer → TextureHardwareBuffer shared_ptr)
    std::unordered_map<std::string,
        std::unordered_map<uintptr_t, TextureHardwareBufferPtr>> mTextureHardwareBufferCache;
};

} // namespace renderer
} // namespace engine

/// 便捷访问宏
#define AssetManager framework::Singleton<engine::renderer::AssetManager>::getInstance()

#endif // LEARNOPENGLES_ASSETMANAGER_H