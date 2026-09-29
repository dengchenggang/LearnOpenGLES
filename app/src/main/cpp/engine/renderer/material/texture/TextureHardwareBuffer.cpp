#include "TextureHardwareBuffer.h"
#include "RenderInterface.h"
#include "utils/Log.h"

#include <android/hardware_buffer.h>

KEY_VALUE(TAG, TextureHardwareBuffer)

namespace engine {
namespace renderer {

// ===================================================================
// 构造 / 析构
// ===================================================================

TextureHardwareBuffer::TextureHardwareBuffer() : Texture(false) {}

TextureHardwareBuffer::~TextureHardwareBuffer() {
    release();
}

// ===================================================================
// 创建
// ===================================================================

bool TextureHardwareBuffer::create(AHardwareBuffer* buffer,
                                   TextureFilter minFilter,
                                   TextureFilter magFilter,
                                   TextureWrap   wrapS,
                                   TextureWrap   wrapT) {
    if (isValid()) {
        LogE("%s already created, duplicate create is not allowed", TAG);
        return false;
    }

    if (!buffer) {
        LogE("%s buffer is null", TAG);
        return false;
    }

    mDesc.minFilter = minFilter;
    mDesc.magFilter = magFilter;
    mDesc.wrapS     = wrapS;
    mDesc.wrapT     = wrapT;

    AHardwareBuffer_Desc hwDesc;
    AHardwareBuffer_describe(buffer, &hwDesc);
    mDesc.width  = hwDesc.width;
    mDesc.height = hwDesc.height;

    // 格式映射：仅支持 RGB888 / RGBA8888
    switch (hwDesc.format) {
        case AHARDWAREBUFFER_FORMAT_R8G8B8_UNORM:
            mDesc.format = TextureFormat::RGB8;
            break;
        case AHARDWAREBUFFER_FORMAT_R8G8B8A8_UNORM:
            mDesc.format = TextureFormat::RGBA8;
            break;
        default:
            LogE("%s unsupported AHardwareBuffer format: %d", TAG, hwDesc.format);
            return false;
    }

    // 创建 GL 纹理
    mHandle = RenderInterface.genTexture2D();
    if (mHandle == INVALID_HANDLE) {
        LogE("%s genTexture2D failed", TAG);
        return false;
    }

    // 创建 EGLImage 并绑定到 GL 纹理
    if (!createAndBindImage(buffer)) {
        RenderInterface.deleteTexture(mHandle);
        mHandle = INVALID_HANDLE;
        return false;
    }

    LogD("%s created: tex=%u, size=%dx%d", TAG, mHandle, mDesc.width, mDesc.height);
    return true;
}

// ===================================================================
// 内部：从 AHardwareBuffer 创建 EGLImage 并绑定到当前 GL 纹理
// ===================================================================

bool TextureHardwareBuffer::createAndBindImage(AHardwareBuffer* buffer) {
    mImage = RenderInterface.createImageKHR(buffer);
    if (!mImage) {
        LogE("%s createImageKHR failed", TAG);
        return false;
    }

    bind();
    RenderInterface.bindImageToTexture2D(mImage);
    setFilter(mDesc.minFilter, mDesc.magFilter);
    setWrap(mDesc.wrapS, mDesc.wrapT);
    unbind();

    return true;
}

// ===================================================================
// 内部：释放所有资源
// ===================================================================

void TextureHardwareBuffer::release() {
    if (mHandle != INVALID_HANDLE) {
        RenderInterface.deleteTexture(mHandle);
        mHandle = INVALID_HANDLE;
    }

    if (mImage) {
        RenderInterface.destroyImageKHR(mImage);
        mImage = nullptr;
    }

    mDesc = {};
}

} // namespace renderer
} // namespace engine
