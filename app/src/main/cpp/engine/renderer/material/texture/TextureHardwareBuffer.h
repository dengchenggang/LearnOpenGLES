#ifndef LEARNOPENGLES_TEXTUREHARDWAREBUFFER_H
#define LEARNOPENGLES_TEXTUREHARDWAREBUFFER_H

#include "Texture.h"
#include <cstdint>
#include <memory>

#include <android/hardware_buffer.h>

namespace engine {
namespace renderer {

/// TextureHardwareBuffer：EGLImage + GL 纹理 的 RAII 包装
///
/// 从 AHardwareBuffer 创建 EGLImage 并挂载到 GL 纹理（零拷贝）。
/// 所有 EGL/GL 调用全部通过 RenderInterface，不在本类中直接调用原生 API。
///
/// 典型使用：
/// @code
///   void onHardwareBuffer(const VideoHardwareBufferPtr& hwBuf) {
///       TextureHardwareBuffer texBuffer;
///       if (!texBuffer.create(hwBuf->get())) return;
///       texBuffer.bind(0);
///       // ... 绘制 ...
///       texBuffer.unbind(0);
///   }
/// @endcode
class TextureHardwareBuffer : public Texture {
public:
    /// 默认构造：不分配任何 GL/EGL 资源
    TextureHardwareBuffer();

    /// 析构：销毁 EGLImage + GL 纹理
    ~TextureHardwareBuffer() override;

    // ---- 禁止拷贝和移动 ----
    TextureHardwareBuffer(const TextureHardwareBuffer&) = delete;
    TextureHardwareBuffer& operator=(const TextureHardwareBuffer&) = delete;
    TextureHardwareBuffer(TextureHardwareBuffer&&) = delete;
    TextureHardwareBuffer& operator=(TextureHardwareBuffer&&) = delete;
public:
    /// 从 AHardwareBuffer 创建：创建 EGLImage + GL 纹理并绑定
    /// 只允许创建一次，重复调用会报错返回 false。
    /// @param buffer     有效的 AHardwareBuffer*
    /// @param minFilter  缩小过滤模式
    /// @param magFilter  放大过滤模式
    /// @param wrapS      S 轴环绕模式
    /// @param wrapT      T 轴环绕模式
    /// @return 成功返回 true
    bool create(AHardwareBuffer* buffer,
                TextureFilter minFilter = TextureFilter::Linear,
                TextureFilter magFilter = TextureFilter::Linear,
                TextureWrap   wrapS     = TextureWrap::ClampToEdge,
                TextureWrap   wrapT     = TextureWrap::ClampToEdge);

    bool isValid() const override { return mHandle != INVALID_HANDLE && mImage != nullptr; }

    /// 转换为 bool（等价于 isValid()）
    explicit operator bool() const { return isValid(); }

private:
    void release();
    bool createAndBindImage(AHardwareBuffer* buffer);

    void* mImage = nullptr;   // EGLImageKHR 句柄
};

using TextureHardwareBufferPtr = std::shared_ptr<TextureHardwareBuffer>;

} // namespace renderer
} // namespace engine

#endif // LEARNOPENGLES_TEXTUREHARDWAREBUFFER_H
