#include "Texture.h"
#include "RenderInterface.h"

namespace engine {


Texture::Texture() = default;

Texture::~Texture() {
    destroy();
}

Texture::Texture(Texture&& other) noexcept
    : mHandle(other.mHandle)
    , mWidth(other.mWidth)
    , mHeight(other.mHeight)
    , mFormat(other.mFormat)
    , mMinFilter(other.mMinFilter)
    , mMagFilter(other.mMagFilter)
    , mWrapS(other.mWrapS)
    , mWrapT(other.mWrapT)
    , mHasMipmap(other.mHasMipmap) {
    other.mHandle = INVALID_HANDLE;
    other.mWidth = 0;
    other.mHeight = 0;
}

Texture& Texture::operator=(Texture&& other) noexcept {
    if (this != &other) {
        destroy();

        mHandle = other.mHandle;
        mWidth = other.mWidth;
        mHeight = other.mHeight;
        mFormat = other.mFormat;
        mMinFilter = other.mMinFilter;
        mMagFilter = other.mMagFilter;
        mWrapS = other.mWrapS;
        mWrapT = other.mWrapT;
        mHasMipmap = other.mHasMipmap;

        other.mHandle = INVALID_HANDLE;
        other.mWidth = 0;
        other.mHeight = 0;
    }
    return *this;
}

bool Texture::create(const TextureDesc& desc) {
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
    mWidth = desc.width;
    mHeight = desc.height;
    mFormat = desc.format;
    mMinFilter = desc.minFilter;
    mMagFilter = desc.magFilter;
    mWrapS = desc.wrapS;
    mWrapT = desc.wrapT;

    // 绑定并设置纹理参数
    bind(0);
    setFilter(mMinFilter, mMagFilter);
    setWrap(mWrapS, mWrapT);

    // 生成 Mipmap
    if (desc.generateMipmap) {
        generateMipmap();
    }

    return true;
}

bool Texture::create(int32_t width, int32_t height, TextureFormat format, const void* data) {
    TextureDesc desc;
    desc.width = width;
    desc.height = height;
    desc.format = format;
    desc.initialData = data;
    return create(desc);
}

void Texture::destroy() {
    if (mHandle != INVALID_HANDLE) {
        RenderInterface.deleteTexture(mHandle);
        mHandle = INVALID_HANDLE;
        mWidth = 0;
        mHeight = 0;
        mHasMipmap = false;
    }
}

void Texture::bind(uint32_t unit) const {
    if (mHandle != INVALID_HANDLE) {
        RenderInterface.activeTextureUnit(unit);
        RenderInterface.bindTexture2D(mHandle, unit);
    }
}

void Texture::unbind(uint32_t unit) {
    RenderInterface.activeTextureUnit(unit);
    RenderInterface.bindTexture2D(INVALID_HANDLE, unit);
}

void Texture::updateData(const void* data, int32_t x, int32_t y, int32_t width, int32_t height) {
    if (!data || mHandle == INVALID_HANDLE) {
        return;
    }

    // 默认更新整个纹理
    if (width < 0) width = mWidth - x;
    if (height < 0) height = mHeight - y;

    if (width <= 0 || height <= 0) {
        return;
    }

    // 使用渲染接口更新纹理数据
    RenderInterface.updateTexture2D(mHandle, x, y, width, height, data);
}

void Texture::generateMipmap() {
    if (mHandle != INVALID_HANDLE) {
        bind(0);
        RenderInterface.generateMipmap();
        mHasMipmap = true;
    }
}

void Texture::setFilter(TextureFilter minFilter, TextureFilter magFilter) {
    mMinFilter = minFilter;
    mMagFilter = magFilter;

    if (mHandle != INVALID_HANDLE) {
        bind(0);
        RenderInterface.setTextureFilter(minFilter, magFilter);
    }
}

void Texture::setWrap(TextureWrap wrapS, TextureWrap wrapT) {
    mWrapS = wrapS;
    mWrapT = wrapT;

    if (mHandle != INVALID_HANDLE) {
        bind(0);
        RenderInterface.setTextureWrap(wrapS, wrapT);
    }
}

uint32_t Texture::getActiveTextureUnit() {
    return RenderInterface.getActiveTextureUnit();
}

} // namespace engine
