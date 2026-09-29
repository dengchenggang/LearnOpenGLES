#ifndef TEXTURE_H
#define TEXTURE_H

#include "IRenderInterface.h"
#include <memory>

namespace engine {
namespace renderer {

struct TextureDesc {
    int32_t width = 0;
    int32_t height = 0;
    TextureFormat format = TextureFormat::RGBA8;
    TextureFilter minFilter = TextureFilter::Linear;
    TextureFilter magFilter = TextureFilter::Linear;
    TextureWrap wrapS = TextureWrap::ClampToEdge;
    TextureWrap wrapT = TextureWrap::ClampToEdge;
    bool generateMipmap = false;
    const void* initialData = nullptr;
};

// 纹理基类：定义所有纹理的公共接口
class Texture {
public:
// 禁止拷贝
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    // 禁止移动
    Texture(Texture&&) = delete;
    Texture& operator=(Texture&&) = delete;

    virtual ~Texture() = default;
public:
    // 绑定纹理到指定纹理单元
    virtual void bind(uint32_t unit = 0) const;

    // 解绑纹理
    static void unbind(uint32_t unit = 0);

    // 设置过滤模式
    void setFilter(TextureFilter minFilter, TextureFilter magFilter);

    // 设置环绕模式
    void setWrap(TextureWrap wrapS, TextureWrap wrapT);

    // 生成 Mipmap（仅当 mSupportsMipmap 为 true 时生效）
    void generateMipmap();

    // 查询
    RenderResourceHandle getHandle() const { return mHandle; }
    int32_t getWidth() const { return mDesc.width; }
    int32_t getHeight() const { return mDesc.height; }
    TextureFormat getFormat() const { return mDesc.format; }
    bool hasMipmap() const { return mHasMipmap; }
    virtual bool isValid() const { return mHandle != INVALID_HANDLE; }

    // 获取当前绑定的纹理单元
    static uint32_t getActiveTextureUnit();
protected:
    explicit Texture(bool supportsMipmap);
protected:
    TextureDesc mDesc;
    bool mHasMipmap = false;
    const bool mSupportsMipmap;
    RenderResourceHandle mHandle = INVALID_HANDLE;
};

using TexturePtr = std::unique_ptr<Texture>;
using TextureSharedPtr = std::shared_ptr<Texture>;

} // namespace renderer
} // namespace engine

#endif // TEXTURE_H
