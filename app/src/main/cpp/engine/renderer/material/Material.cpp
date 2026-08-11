#include "Material.h"
#include <stdexcept>

namespace engine {
namespace renderer {


void Material::setTexture(std::shared_ptr<Texture> texture, uint32_t unit) {
    for (auto& slot : mTextures) {
        if (slot.unit == unit) {
            slot.texture = std::move(texture);
            return;
        }
    }
    mTextures.push_back({std::move(texture), unit});
}

Texture& Material::getTexture(uint32_t unit) {
    for (auto& slot : mTextures) {
        if (slot.unit == unit) {
            return *slot.texture;
        }
    }
    throw std::runtime_error("Texture not found");
}

const Texture& Material::getTexture(uint32_t unit) const {
    for (const auto& slot : mTextures) {
        if (slot.unit == unit) {
            return *slot.texture;
        }
    }
    throw std::runtime_error("Texture not found");
}

void Material::bind() const {
    if (mShader) {
        mShader->bind();
    }
    for (const auto& slot : mTextures) {
        if (slot.texture) {
            slot.texture->bind(slot.unit);
        }
    }
}

void Material::unbind() const {
    for (const auto& slot : mTextures) {
        Texture::unbind(slot.unit);
    }
    if (mShader) {
        mShader->unbind();
    }
}

void Material::setUniformInt(const std::string& name, int32_t value) const {
    if (mShader) mShader->setUniformInt(name, value);
}

void Material::setUniformFloat(const std::string& name, float value) const {
    if (mShader) mShader->setUniformFloat(name, value);
}

void Material::setUniformVec2(const std::string& name, float x, float y) const {
    if (mShader) mShader->setUniformVec2(name, x, y);
}

void Material::setUniformVec3(const std::string& name, float x, float y, float z) const {
    if (mShader) mShader->setUniformVec3(name, x, y, z);
}

void Material::setUniformVec4(const std::string& name, float x, float y, float z, float w) const {
    if (mShader) mShader->setUniformVec4(name, x, y, z, w);
}

void Material::setUniformMat4(const std::string& name, const float* matrix, bool transpose) const {
    if (mShader) mShader->setUniformMat4(name, matrix, transpose);
}

} // namespace renderer
} // namespace engine
