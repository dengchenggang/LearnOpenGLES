#ifndef TEXTURE_H
#define TEXTURE_H

#include "IRenderInterface.h"
#include <memory>

namespace engine {
namespace renderer {


// 纹理描述信息
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

// 纹理类：封装 OpenGL ES 纹理对象
class Texture {
public:
    Texture();
    ~Texture();

    // 禁止拷贝，允许移动
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;

    // 创建 2D 纹理
    bool create(const TextureDesc& desc);

    // 从现有数据创建 (便捷接口)
    bool create(int32_t width, int32_t height, TextureFormat format, const void* data = nullptr);

    // 销毁纹理
    void destroy();

    // 绑定纹理到指定纹理单元
    void bind(uint32_t unit = 0) const;

    // 解绑纹理
    static void unbind(uint32_t unit = 0);

    // 更新纹理数据
    void updateData(const void* data, int32_t x = 0, int32_t y = 0, int32_t width = -1, int32_t height = -1);

    // 生成 Mipmap
    void generateMipmap();

    // 设置过滤模式
    void setFilter(TextureFilter minFilter, TextureFilter magFilter);

    // 设置环绕模式
    void setWrap(TextureWrap wrapS, TextureWrap wrapT);

    // 获取纹理句柄
    RenderResourceHandle getHandle() const { return mHandle; }

    // 获取纹理尺寸
    int32_t getWidth() const { return mWidth; }
    int32_t getHeight() const { return mHeight; }
    TextureFormat getFormat() const { return mFormat; }

    // 检查纹理是否有效
    bool isValid() const { return mHandle != INVALID_HANDLE; }

    // 获取当前绑定的纹理单元 (用于恢复状态)
    static uint32_t getActiveTextureUnit();

private:
    RenderResourceHandle mHandle = INVALID_HANDLE;
    int32_t mWidth = 0;
    int32_t mHeight = 0;
    TextureFormat mFormat = TextureFormat::RGBA8;
    TextureFilter mMinFilter = TextureFilter::Linear;
    TextureFilter mMagFilter = TextureFilter::Linear;
    TextureWrap mWrapS = TextureWrap::ClampToEdge;
    TextureWrap mWrapT = TextureWrap::ClampToEdge;
    bool mHasMipmap = false;
};

// 纹理智能指针类型
using TexturePtr = std::unique_ptr<Texture>;
using TextureSharedPtr = std::shared_ptr<Texture>;


} // namespace renderer
} // namespace engine

#endif // TEXTURE_H