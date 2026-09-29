#include "Texture2D.h"
#include "RenderInterface.h"

namespace engine {
namespace renderer {

Texture2D::Texture2D() : Texture(true) {}

Texture2D::~Texture2D() {
    destroy();
}

bool Texture2D::create(const TextureDesc& desc) {
    if (desc.width <= 0 || desc.height <= 0) {
        return false;
    }

    // 销毁旧纹理
    destroy();

    // 创建纹理
    mHandle = RenderInterface.createTexture2D(desc.width, desc.height, desc.format, desc.initialData);
    if (mHandle == INVALID_HANDLE) {
        return false;
    }

    // 保存属性
    mDesc = desc;

    // 绑定并设置纹理参数
    bind();
    setFilter(mDesc.minFilter, mDesc.magFilter);
    setWrap(mDesc.wrapS, mDesc.wrapT);

    // 生成 Mipmap
    if (desc.generateMipmap) {
        generateMipmap();
    }

    unbind();
    return true;
}

bool Texture2D::create(int32_t width, int32_t height, TextureFormat format, const void* data) {
    TextureDesc desc;
    desc.width = width;
    desc.height = height;
    desc.format = format;
    desc.initialData = data;
    return create(desc);
}

void Texture2D::destroy() {
    if (mHandle != INVALID_HANDLE) {
        RenderInterface.deleteTexture(mHandle);
        mHandle = INVALID_HANDLE;
        mDesc = {};
        mHasMipmap = false;
    }
}

void Texture2D::updateData(const void* data, int32_t x, int32_t y, int32_t width, int32_t height) {
    if (!data || mHandle == INVALID_HANDLE) {
        return;
    }

    // 默认更新整个纹理
    if (width < 0) width = mDesc.width - x;
    if (height < 0) height = mDesc.height - y;

    if (width <= 0 || height <= 0) {
        return;
    }

    // 使用渲染接口更新纹理数据
    RenderInterface.updateTexture2D(mHandle, mDesc.format, x, y, width, height, data);
}

} // namespace renderer
} // namespace engine
