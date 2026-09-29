#ifndef TEXTURE2D_H
#define TEXTURE2D_H

#include "Texture.h"

namespace engine {
namespace renderer {

// 2D 纹理：从 CPU 像素数据创建
class Texture2D : public Texture {
public:
    Texture2D();
    ~Texture2D() override;

    // 禁止拷贝和移动
    Texture2D(const Texture2D&) = delete;
    Texture2D& operator=(const Texture2D&) = delete;
    Texture2D(Texture2D&&) = delete;
    Texture2D& operator=(Texture2D&&) = delete;

    // 创建 2D 纹理
    bool create(const TextureDesc& desc);
    bool create(int32_t width, int32_t height, TextureFormat format, const void* data = nullptr);

    // 销毁纹理
    void destroy();

    // 更新纹理数据
    void updateData(const void* data, int32_t x = 0, int32_t y = 0, int32_t width = -1, int32_t height = -1);

private:
};

using Texture2DPtr = std::unique_ptr<Texture2D>;
using Texture2DSharedPtr = std::shared_ptr<Texture2D>;

} // namespace renderer
} // namespace engine

#endif // TEXTURE2D_H
