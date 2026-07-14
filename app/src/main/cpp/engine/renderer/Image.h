#ifndef IMAGE_H
#define IMAGE_H

#include "IRenderer.h"
#include "Shader.h"
#include "Texture.h"
#include <glm/glm.hpp>
#include <memory>
#include <string>

// 图像渲染器：使用 OpenGL ES 渲染 2D 图像/纹理
class Image : public IRenderer {
public:
    Image();
    ~Image() override;

    // 禁止拷贝和移动
    Image(const Image&) = delete;
    Image& operator=(const Image&) = delete;
    Image(Image&&) = delete;
    Image& operator=(Image&&) = delete;

    // IRenderer 接口实现
    void initialize() override;
    void update(int64_t deltaTime) override;
    void render(int64_t deltaTime) override;
    void release() override;

    // 加载图像（从内存数据）
    bool load(const uint8_t* buffer, int32_t width, int32_t height, int32_t channels);

    // 设置位置（屏幕坐标，左下角为原点）
    void setPosition(float x, float y);

    // 设置缩放比例（基于图像原始大小）
    void setScale(float scaleX, float scaleY);

    // 设置颜色叠加（乘色）
    void setColor(float r, float g, float b, float a);

    // 获取当前位置
    glm::vec2 getPosition() const { return mPosition; }

    // 获取图像原始大小
    glm::vec2 getImageSize() const { return mImageSize; }

    // 获取当前缩放比例
    glm::vec2 getScale() const { return mScale; }

    // 检查是否有效
    bool isValid() const;

private:
    // 着色器和纹理
    std::unique_ptr<Shader> mShader;
    std::unique_ptr<Texture> mTexture;

    // 顶点数组和缓冲区
    RenderResourceHandle mVAO = INVALID_HANDLE;
    RenderResourceHandle mVBO = INVALID_HANDLE;
    RenderResourceHandle mEBO = INVALID_HANDLE;

    // 变换属性
    glm::vec2 mPosition = glm::vec2(0.0f, 0.0f);
    glm::vec2 mScale = glm::vec2(1.0f, 1.0f);
    glm::vec2 mImageSize = glm::vec2(1.0f, 1.0f);
    glm::vec4 mColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    // 是否已初始化
    bool mInitialized = false;

    // 创建网格
    void createMesh();

    // 更新变换矩阵
    void updateTransform();

    // 加载默认着色器
    bool loadDefaultShader();
};

#endif // IMAGE_H
