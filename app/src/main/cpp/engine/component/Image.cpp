#include "Image.h"
#include "filesystem/FileSystem.h"
#include "RenderInterface.h"
#include "utils/Log.h"
#include <glm/gtc/matrix_transform.hpp>

namespace engine {

using namespace renderer;

KEY_VALUE(TAG, Image)

Image::Image(Actor& owner)
    : CSceneComponent(owner, std::make_unique<RectTransform>())
    , mRectTransform(static_cast<RectTransform&>(getTransform())) {}

Image::~Image() {
    onEndPlay();
}

void Image::onBeginPlay() {
    if (mInitialized) {
        LogW("%s already initialized", TAG);
        return;
    }

    LogI("%s initializing...", TAG);

    // 创建默认材质和着色器
    if (!loadDefaultShader()) {
        LogE("%s failed to load default shader", TAG);
        return;
    }

    // 创建网格
    createMesh();

    mInitialized = true;
    LogI("%s initialized successfully", TAG);
}

void Image::onUpdate(float deltaTime) {
    // 更新逻辑（如动画）
}

void Image::onRender() {
    if (!mInitialized || !mMesh || !mMesh->isValid() || !mMaterial || !mMaterial->isValid()) {
        return;
    }

    // 更新变换矩阵和颜色
    updateTransform();

    // 绑定材质并绘制
    mMaterial->bind();
    mMesh->draw();
    mMaterial->unbind();
}

void Image::onEndPlay() {
    LogI("%s releasing...", TAG);

    mMesh.reset();
    mMaterial.reset();

    mInitialized = false;
    LogI("%s released", TAG);
}

bool Image::load(const uint8_t* buffer, int32_t width, int32_t height, int32_t channels) {
    if (!mInitialized) {
        LogE("%s not initialized, cannot load image", TAG);
        return false;
    }

    if (!buffer) {
        LogE("%s buffer is null", TAG);
        return false;
    }

    if (width <= 0 || height <= 0) {
        LogE("%s invalid image size: %dx%d", TAG, width, height);
        return false;
    }

    LogI("%s loading image: %dx%d, channels: %d", TAG, width, height, channels);

    // 根据通道数选择纹理格式
    TextureFormat format;
    switch (channels) {
        case 3:
            format = TextureFormat::RGB8;
            break;
        case 4:
            format = TextureFormat::RGBA8;
            break;
        default:
            LogE("%s unsupported channels: %d (only 3 or 4 supported)", TAG, channels);
            return false;
    }

    // 创建纹理
    auto texture = std::make_shared<Texture>();

    TextureDesc desc;
    desc.width = width;
    desc.height = height;
    desc.format = format;
    desc.initialData = buffer;

    if (!texture->create(desc)) {
        LogE("%s failed to create texture: %dx%d", TAG, width, height);
        return false;
    }

    mMaterial->setTexture(std::move(texture), 0);

    LogI("%s texture created successfully: %dx%d", TAG, width, height);

    // 保存图像原始大小
    mImageSize = glm::vec2(static_cast<float>(width), static_cast<float>(height));

    LogI("%s image loaded successfully (size: %.0fx%.0f)", TAG, mImageSize.x, mImageSize.y);
    return true;
}

void Image::setColor(float r, float g, float b, float a) {
    mColor = glm::vec4(r, g, b, a);
}

bool Image::isValid() const {
    return mInitialized && mMesh && mMesh->isValid() &&
           mMaterial && mMaterial->isValid();
}

void Image::createMesh() {
    mMesh = std::make_shared<Mesh>();

    // 位置数据（SOA 布局）
    float positions[] = {
        0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f
    };

    // 纹理坐标数据（SOA 布局）
    float texCoords[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };

    // 索引数据
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
}

void Image::updateTransform() {
    // 从 RectTransform 获取模型矩阵
    const glm::mat4& model = mRectTransform.getModelMatrix();

    // 从 RenderInterface 获取视口尺寸并构建正交投影矩阵
    int32_t x, viewportWidth, viewportHeight;
    RenderInterface.getViewport(&x, &x, &viewportWidth, &viewportHeight);
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(viewportWidth),
                                      0.0f, static_cast<float>(viewportHeight),
                                      -1.0f, 1.0f);

    // 设置 Uniform
    mMaterial->setUniformMat4("uModelMatrix", model);
    mMaterial->setUniformMat4("uViewMatrix", glm::mat4(1.0f));
    mMaterial->setUniformMat4("uProjectionMatrix", projection);
    mMaterial->setUniformVec4("uColor", mColor.r, mColor.g, mColor.b, mColor.a);
    mMaterial->setUniformInt("uTexture", 0);
}

bool Image::loadDefaultShader() {
    LogI("%s loading default shader...", TAG);

    // 创建着色器
    auto shader = std::make_shared<Shader>("texture");

    // 从文件系统读取着色器源码
    std::string vertSource = FileSystem.readString("shader/texture.vert.glsl");
    std::string fragSource = FileSystem.readString("shader/texture.frag.glsl");

    if (vertSource.empty() || fragSource.empty()) {
        LogE("%s shader files not found: shader/texture.vert.glsl or shader/texture.frag.glsl", TAG);
        return false;
    }

    // 编译着色器
    if (!shader->compile(Shader::Type::Vertex, vertSource)) {
        LogE("%s failed to compile vertex shader", TAG);
        return false;
    }
    LogD("%s vertex shader compiled successfully", TAG);

    if (!shader->compile(Shader::Type::Fragment, fragSource)) {
        LogE("%s failed to compile fragment shader", TAG);
        return false;
    }
    LogD("%s fragment shader compiled successfully", TAG);

    // 链接着色器程序
    if (!shader->link()) {
        LogE("%s failed to link shader program", TAG);
        return false;
    }

    mMaterial = std::make_shared<Material>();
    mMaterial->setShader(std::move(shader));

    LogI("%s shader program linked successfully", TAG);
    return true;
}

} // namespace engine
