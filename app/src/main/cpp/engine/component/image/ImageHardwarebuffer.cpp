#include "ImageHardwarebuffer.h"
#include "AssetManager.h"
#include "filesystem/FileSystem.h"
#include "RenderInterface.h"
#include "utils/Log.h"

#include <glm/gtc/matrix_transform.hpp>

namespace engine {

using namespace renderer;

KEY_VALUE(TAG, ImageHardwarebuffer)

// ---------------------------------------------------------------------------
// 构造 / 析构
// ---------------------------------------------------------------------------

ImageHardwarebuffer::ImageHardwarebuffer(Actor& owner)
    : SceneComponent(owner, std::make_unique<RectTransform>())
    , mRectTransform(static_cast<RectTransform&>(getTransform())) {}

ImageHardwarebuffer::~ImageHardwarebuffer() {
    onEndPlay();
}

// ---------------------------------------------------------------------------
// 生命周期
// ---------------------------------------------------------------------------

void ImageHardwarebuffer::onBeginPlay() {
    if (mInitialized) {
        LogW("%s already initialized", TAG);
        return;
    }

    LogI("%s initializing...", TAG);

    if (!loadDefaultShader()) {
        LogE("%s failed to load default shader", TAG);
        return;
    }

    createMesh();

    mInitialized = true;
    LogI("%s initialized successfully", TAG);
}

void ImageHardwarebuffer::onUpdate(float /*deltaTime*/) {
    // 预留动画等更新逻辑
}

void ImageHardwarebuffer::onRender() {
    if (!mInitialized) {
        LogW("%s not initialized, skip render", TAG);
        return;
    }
    if (!mTexture || !mTexture->isValid()) {
        return;
    }
    if (!mMesh || !mMesh->isValid()) {
        LogW("%s mesh invalid, skip render", TAG);
        return;
    }
    if (!mMaterial || !mMaterial->isValid()) {
        LogW("%s material invalid, skip render", TAG);
        return;
    }

    // 1) 绑定着色器程序
    mMaterial->bind();

    // 2) 绑定硬件纹理（从 AssetManager 获取，指针级缓存复用）
    mTexture->bind(0);

    // 3) 设置变换 + 颜色 uniform
    updateTransform();

    // 4) 绘制
    mMesh->draw();

    // 5) 解绑
    TextureHardwareBuffer::unbind(0);
    mMaterial->unbind();
}

void ImageHardwarebuffer::onEndPlay() {
    LogI("%s releasing...", TAG);

    mTexture.reset();

    mMesh.reset();
    mMaterial.reset();

    mInitialized = false;
    LogI("%s released", TAG);
}

// ---------------------------------------------------------------------------
// 公共接口
// ---------------------------------------------------------------------------

bool ImageHardwarebuffer::load(const std::string& url, AHardwareBuffer* buffer) {
    if (!mInitialized) {
        LogE("%s not initialized, cannot load", TAG);
        return false;
    }

    if (!buffer) {
        LogW("%s buffer is null", TAG);
        return false;
    }

    mTexture = AssetManager.loadTexture(url, buffer);
    if (!mTexture) {
        LogE("%s AssetManager.loadTexture failed: url=%s", TAG, url.c_str());
        return false;
    }

    // 更新尺寸（buffer 可能换了解析率）
    mImageSize = glm::vec2(static_cast<float>(mTexture->getWidth()),
                           static_cast<float>(mTexture->getHeight()));

    return true;
}

void ImageHardwarebuffer::setColor(float r, float g, float b, float a) {
    mColor = glm::vec4(r, g, b, a);
}

bool ImageHardwarebuffer::isValid() const {
    return mInitialized
           && mMesh && mMesh->isValid()
           && mMaterial && mMaterial->isValid()
           && mTexture && mTexture->isValid();
}

// ---------------------------------------------------------------------------
// Mesh
// ---------------------------------------------------------------------------

void ImageHardwarebuffer::createMesh() {
    mMesh = std::make_shared<Mesh>();

    float positions[] = {
        0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f
    };

    float texCoords[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };

    uint16_t indices[] = {
        0, 1, 2,
        0, 2, 3
    };

    mMesh->initialize({
        {Attr::Position(0), 4},
        {Attr::TexCoord(1), 4}
    });
    mMesh->setAttributeData(0, positions, sizeof(positions));
    mMesh->setAttributeData(1, texCoords, sizeof(texCoords));
    mMesh->setIndexData(indices, sizeof(indices), DataType::UShort, 6);

    LogI("%s mesh created: valid=%d", TAG, mMesh->isValid());
}

// ---------------------------------------------------------------------------
// Transform
// ---------------------------------------------------------------------------

void ImageHardwarebuffer::updateTransform() {
    glm::mat4 model = getWorldMatrix();

    int32_t x, y, viewportWidth, viewportHeight;
    RenderInterface.getViewport(&x, &y, &viewportWidth, &viewportHeight);
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(viewportWidth),
                                      0.0f, static_cast<float>(viewportHeight),
                                      -1.0f, 1.0f);

    auto pos = mRectTransform.getPosition();
    auto size = mRectTransform.getSize();
    LogD("%s render: pos=(%.1f,%.1f,%.1f) size=(%.1f,%.1f) viewport=%dx%d",
         TAG, pos.x, pos.y, pos.z, size.x, size.y, viewportWidth, viewportHeight);

    mMaterial->setUniformMat4("uModelMatrix", model);
    mMaterial->setUniformMat4("uViewMatrix", glm::mat4(1.0f));
    mMaterial->setUniformMat4("uProjectionMatrix", projection);
    mMaterial->setUniformVec4("uColor", mColor.r, mColor.g, mColor.b, mColor.a);
}

// ---------------------------------------------------------------------------
// Shader
// ---------------------------------------------------------------------------

bool ImageHardwarebuffer::loadDefaultShader() {
    LogI("%s loading default shader...", TAG);

    auto shader = std::make_shared<Shader>("texture");

    std::string vertSource = FileSystem.readString("shader/texture.vert.glsl");
    std::string fragSource = FileSystem.readString("shader/texture.frag.glsl");

    if (vertSource.empty() || fragSource.empty()) {
        LogE("%s shader files not found", TAG);
        return false;
    }

    if (!shader->compile(Shader::Type::Vertex, vertSource)) {
        LogE("%s failed to compile vertex shader", TAG);
        return false;
    }
    LogD("%s vertex shader compiled", TAG);

    if (!shader->compile(Shader::Type::Fragment, fragSource)) {
        LogE("%s failed to compile fragment shader", TAG);
        return false;
    }
    LogD("%s fragment shader compiled", TAG);

    if (!shader->link()) {
        LogE("%s failed to link shader program", TAG);
        return false;
    }

    mMaterial = std::make_shared<Material>();
    mMaterial->setShader(std::move(shader));

    LogI("%s shader linked", TAG);
    return true;
}

} // namespace engine
