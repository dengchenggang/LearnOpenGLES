#ifndef IMAGE_H
#define IMAGE_H

#include "CSceneComponent.h"
#include "RectTransform.h"
#include "Material.h"
#include "Mesh.h"
#include <glm/glm.hpp>
#include <memory>
#include <string>

namespace engine {

using namespace renderer;

// 图像渲染器：使用 OpenGL ES 渲染 2D 图像/纹理
class Image : public CSceneComponent {
public:
    explicit Image(Actor& owner);
    ~Image() override;

    Image(const Image&) = delete;
    Image& operator=(const Image&) = delete;
    Image(Image&&) = delete;
    Image& operator=(Image&&) = delete;
public:
    void onBeginPlay() override;
    void onUpdate(float deltaTime) override;
    void onRender() override;
    void onEndPlay() override;

    RectTransform& getRectTransform() const { return mRectTransform; }
    // 加载图像（从内存数据）
    bool load(const uint8_t* buffer, int32_t width, int32_t height, int32_t channels);

    // 设置颜色叠加（乘色）
    void setColor(float r, float g, float b, float a);

    // 获取图像原始大小
    glm::vec2 getImageSize() const { return mImageSize; }

    // 检查是否有效
    bool isValid() const;

private:
    RectTransform& mRectTransform;
    // 网格和材质
    std::shared_ptr<Mesh> mMesh;
    std::shared_ptr<Material> mMaterial;

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


} // namespace engine

#endif // IMAGE_H
