#ifndef MATERIAL_H
#define MATERIAL_H

#include "Shader.h"
#include "Texture.h"
#include <memory>
#include <vector>

// 材质类：管理着色器和纹理
class Material {
public:
    Material() = default;
    ~Material() = default;
    Material(const Material&) = delete;
    Material& operator=(const Material&) = delete;
public:
    // 设置着色器
    void setShader(std::shared_ptr<Shader> shader) { mShader = std::move(shader); }
    Shader& getShader() { return *mShader; }
    const Shader& getShader() const { return *mShader; }

    // 设置纹理
    void setTexture(std::shared_ptr<Texture> texture, uint32_t unit = 0);
    Texture& getTexture(uint32_t unit = 0);
    const Texture& getTexture(uint32_t unit = 0) const;

    // 绑定/解绑
    void bind() const;
    void unbind() const;

    // 设置Uniform（转发到Shader）
    void setUniformInt(const std::string& name, int32_t value) const;
    void setUniformFloat(const std::string& name, float value) const;
    void setUniformVec2(const std::string& name, float x, float y) const;
    void setUniformVec3(const std::string& name, float x, float y, float z) const;
    void setUniformVec4(const std::string& name, float x, float y, float z, float w) const;
    void setUniformMat4(const std::string& name, const float* matrix, bool transpose = false) const;

    // 检查是否有效
    bool isValid() const { return mShader != nullptr && mShader->isValid(); }

private:
    struct TextureSlot {
        std::shared_ptr<Texture> texture;
        uint32_t unit = 0;
    };

    std::shared_ptr<Shader> mShader;
    std::vector<TextureSlot> mTextures;
};

#endif
