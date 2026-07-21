#include "Image.h"
#include "FileSystem.h"
#include "RenderInterface.h"
#include "log/Log.h"
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

KEY_VALUE(TAG, Image)

// 顶点数据结构
struct Vertex {
    float position[3];
    float texCoord[2];
};

Image::Image() = default;

Image::~Image() {
    release();
}

void Image::initialize() {
    if (mInitialized) {
        LogW("%s already initialized", TAG);
        return;
    }

    LogI("%s initializing...", TAG);

    // 创建默认着色器
    if (!loadDefaultShader()) {
        LogE("%s failed to load default shader", TAG);
        return;
    }

    // 创建网格
    createMesh();

    mInitialized = true;
    LogI("%s initialized successfully", TAG);
}

void Image::update(int64_t deltaTime) {
    // 更新逻辑（如动画）
}

void Image::render(int64_t deltaTime) {
    if (!mInitialized || !mShader || !mTexture || !mTexture->isValid()) {
        return;
    }

    // 使用着色器
    mShader->bind();

    // 绑定纹理到单元 0
    mTexture->bind(0);
    mShader->setUniformInt("uTexture", 0);

    // 更新变换矩阵
    updateTransform();

    // 设置颜色
    mShader->setUniformVec4("uColor", mColor);

    // 绑定 VAO 并绘制
    RenderInterface.bindVertexArray(mVAO);
    RenderInterface.drawElements(DrawMode::Triangles, 6, DataType::UShort, nullptr);

    // 解绑
    RenderInterface.bindVertexArray(INVALID_HANDLE);
    mTexture->unbind(0);
    mShader->unbind();
}

void Image::release() {
    LogI("%s releasing...", TAG);

    if (mVAO != INVALID_HANDLE) {
        RenderInterface.deleteVertexArray(mVAO);
        mVAO = INVALID_HANDLE;
    }
    if (mVBO != INVALID_HANDLE) {
        RenderInterface.deleteBuffer(mVBO);
        mVBO = INVALID_HANDLE;
    }
    if (mEBO != INVALID_HANDLE) {
        RenderInterface.deleteBuffer(mEBO);
        mEBO = INVALID_HANDLE;
    }

    mShader.reset();
    mTexture.reset();

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
    mTexture = std::make_unique<Texture>();

    TextureDesc desc;
    desc.width = width;
    desc.height = height;
    desc.format = format;
    desc.initialData = buffer;

    if (!mTexture->create(desc)) {
        LogE("%s failed to create texture: %dx%d", TAG, width, height);
        mTexture.reset();
        return false;
    }

    LogI("%s texture created successfully: %dx%d", TAG, width, height);

    // 保存图像原始大小
    mImageSize = glm::vec2(static_cast<float>(width), static_cast<float>(height));

    LogI("%s image loaded successfully (size: %.0fx%.0f)", TAG, mImageSize.x, mImageSize.y);
    return true;
}

void Image::setPosition(float x, float y) {
    mPosition = glm::vec2(x, y);
}

void Image::setScale(float scaleX, float scaleY) {
    mScale = glm::vec2(scaleX, scaleY);
}

void Image::setColor(float r, float g, float b, float a) {
    mColor = glm::vec4(r, g, b, a);
}

bool Image::isValid() const {
    return mInitialized && mShader && mShader->isValid() &&
           mTexture && mTexture->isValid();
}

void Image::createMesh() {
    // 顶点数据（位置 + 纹理坐标）
    Vertex vertices[] = {
        // 左下角
        {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        // 右下角
        {{1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        // 右上角
        {{1.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
        // 左上角
        {{0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}}
    };

    // 索引数据
    uint16_t indices[] = {
        0, 1, 2,  // 第一个三角形
        0, 2, 3   // 第二个三角形
    };

    // 创建 VAO
    mVAO = RenderInterface.createVertexArray();
    RenderInterface.bindVertexArray(mVAO);

    // 创建并填充 VBO
    mVBO = RenderInterface.createVertexBuffer(vertices, sizeof(vertices), BufferUsage::Static);
    RenderInterface.bindVertexBuffer(mVBO);

    // 创建并填充 EBO
    mEBO = RenderInterface.createIndexBuffer(indices, sizeof(indices), BufferUsage::Static);
    RenderInterface.bindIndexBuffer(mEBO);

    // 设置顶点属性
    // 位置属性 (location = 0)
    RenderInterface.enableVertexAttrib(0);
    RenderInterface.setVertexAttribPointer(0, 3, DataType::Float, false, sizeof(Vertex), nullptr);

    // 纹理坐标属性 (location = 1)
    RenderInterface.enableVertexAttrib(1);
    RenderInterface.setVertexAttribPointer(1, 2, DataType::Float, false, sizeof(Vertex),
                                           reinterpret_cast<void*>(offsetof(Vertex, texCoord)));

    // 解绑
    RenderInterface.bindVertexArray(INVALID_HANDLE);
}

void Image::updateTransform() {
    // 计算实际渲染大小（图像原始大小 × 缩放比例）
    glm::vec2 renderSize = mImageSize * mScale;

    // 构建模型矩阵
    glm::mat4 model = glm::mat4(1.0f);

    // 平移到位置
    model = glm::translate(model, glm::vec3(mPosition, 0.0f));

    // 缩放到实际渲染大小
    model = glm::scale(model, glm::vec3(renderSize, 1.0f));

    // 从 RenderInterface 获取视口尺寸并构建正交投影矩阵
    int32_t x, viewportWidth, viewportHeight;
    RenderInterface.getViewport(&x, &x, &viewportWidth, &viewportHeight);
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(viewportWidth),
                                      0.0f, static_cast<float>(viewportHeight),
                                      -1.0f, 1.0f);

    // 设置 Uniform
    mShader->setUniformMat4("uModelMatrix", model);
    mShader->setUniformMat4("uViewMatrix", glm::mat4(1.0f));
    mShader->setUniformMat4("uProjectionMatrix", projection);
}

bool Image::loadDefaultShader() {
    LogI("%s loading default shader...", TAG);

    // 创建着色器
    mShader = std::make_unique<Shader>("ImageShader");

    // 从文件系统读取着色器源码
    std::string vertSource = FileSystem.readString("shader/texture.vert.glsl");
    std::string fragSource = FileSystem.readString("shader/texture.frag.glsl");

    if (vertSource.empty() || fragSource.empty()) {
        LogE("%s shader files not found: shader/texture.vert.glsl or shader/texture.frag.glsl", TAG);
        return false;
    }

    // 编译着色器
    if (!mShader->compile(Shader::Type::Vertex, vertSource)) {
        LogE("%s failed to compile vertex shader", TAG);
        return false;
    }
    LogD("%s vertex shader compiled successfully", TAG);

    if (!mShader->compile(Shader::Type::Fragment, fragSource)) {
        LogE("%s failed to compile fragment shader", TAG);
        return false;
    }
    LogD("%s fragment shader compiled successfully", TAG);

    // 链接着色器程序
    if (!mShader->link()) {
        LogE("%s failed to link shader program", TAG);
        return false;
    }

    LogI("%s shader program linked successfully", TAG);
    return true;
}
