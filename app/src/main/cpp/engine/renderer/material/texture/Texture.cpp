#include "Texture.h"
#include "RenderInterface.h"

namespace engine {
namespace renderer {

Texture::Texture(bool supportsMipmap) : mSupportsMipmap(supportsMipmap) {}

void Texture::bind(uint32_t unit) const {
    if (mHandle != INVALID_HANDLE) {
        RenderInterface.bindTexture2D(mHandle, unit);
    }
}

void Texture::unbind(uint32_t unit) {
    RenderInterface.bindTexture2D(INVALID_HANDLE, unit);
}

void Texture::setFilter(TextureFilter minFilter, TextureFilter magFilter) {
    mDesc.minFilter = minFilter;
    mDesc.magFilter = magFilter;
    if (isValid()) {
        RenderInterface.setTextureFilter(minFilter, magFilter);
    }
}

void Texture::setWrap(TextureWrap wrapS, TextureWrap wrapT) {
    mDesc.wrapS = wrapS;
    mDesc.wrapT = wrapT;
    if (isValid()) {
        RenderInterface.setTextureWrap(wrapS, wrapT);
    }
}

void Texture::generateMipmap() {
    if (mSupportsMipmap && isValid()) {
        RenderInterface.generateMipmap();
        mHasMipmap = true;
    }
}

uint32_t Texture::getActiveTextureUnit() {
    return RenderInterface.getActiveTextureUnit();
}

} // namespace renderer
} // namespace engine
