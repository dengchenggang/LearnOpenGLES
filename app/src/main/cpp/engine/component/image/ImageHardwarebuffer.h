#ifndef IMAGE_HARDWARE_BUFFER_H
#define IMAGE_HARDWARE_BUFFER_H

#include "SceneComponent.h"
#include "RectTransform.h"
#include "Material.h"
#include "Mesh.h"
#include "texture/TextureHardwareBuffer.h"
#include <glm/glm.hpp>
#include <memory>
#include <string>

namespace framework {
class VideoHardwareBuffer;
using VideoHardwareBufferPtr = std::shared_ptr<VideoHardwareBuffer>;
} // namespace framework

namespace engine {

using namespace renderer;

/// ImageHardwarebuffer：使用 AHardwareBuffer + EGLImage 零拷贝渲染 2D 图像
///
/// 通过 AssetManager 获取 TextureHardwareBuffer，实现 buffer 指针级别复用。
///
/// 典型使用：
/// @code
///   auto img = std::make_shared<ImageHardwarebuffer>(actor);
///   img->onBeginPlay();
///   videoCapture.Connect("camera://20", "view",
///       [img](const VideoHardwareBufferPtr& hwBuf) {
///           img->load("camera://20", hwBuf->get());
///       });
/// @endcode
class ImageHardwarebuffer : public SceneComponent {
public:
    explicit ImageHardwarebuffer(Actor& owner);
    ~ImageHardwarebuffer() override;

    ImageHardwarebuffer(const ImageHardwarebuffer&) = delete;
    ImageHardwarebuffer& operator=(const ImageHardwarebuffer&) = delete;
    ImageHardwarebuffer(ImageHardwarebuffer&&) = delete;
    ImageHardwarebuffer& operator=(ImageHardwarebuffer&&) = delete;

public:
    void onBeginPlay() override;
    void onUpdate(float deltaTime) override;
    void onRender() override;
    void onEndPlay() override;

    RectTransform& getRectTransform() const { return mRectTransform; }

    /// 加载指定 url 下的 AHardwareBuffer 帧数据
    bool load(const std::string& url, AHardwareBuffer* buffer);

    /// 设置颜色叠加（乘色）
    void setColor(float r, float g, float b, float a);

    /// 获取当前缓冲区原始尺寸
    glm::vec2 getImageSize() const { return mImageSize; }

    /// 检查是否有效
    bool isValid() const;

private:
    RectTransform& mRectTransform;
    std::shared_ptr<Mesh> mMesh;
    std::shared_ptr<Material> mMaterial;

    glm::vec2 mImageSize = glm::vec2(1.0f, 1.0f);
    glm::vec4 mColor = glm::vec4(1.0f, 1.0f, 1.0f, 0.3f);

    bool mInitialized = false;

    /// AssetManager 管理的纹理（shared_ptr，与 cache 共享引用计数）
    TextureHardwareBufferPtr mTexture;

    void createMesh();
    void updateTransform();
    bool loadDefaultShader();
};

} // namespace engine

#endif // IMAGE_HARDWARE_BUFFER_H
