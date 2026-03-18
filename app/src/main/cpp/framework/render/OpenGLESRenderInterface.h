#ifndef OPENGLES_RENDER_INTERFACE_H
#define OPENGLES_RENDER_INTERFACE_H

#include "IRenderInterface.h"
#include "Singleton.hpp"
#include <GLES3/gl3.h>
#include <string>
#include <queue>

class OpenGLESRenderInterface : public IRenderInterface {
public:
    OpenGLESRenderInterface() = default;
    ~OpenGLESRenderInterface() override = default;
    // 禁止拷贝
    OpenGLESRenderInterface(const OpenGLESRenderInterface&) = delete;
    OpenGLESRenderInterface& operator=(const OpenGLESRenderInterface&) = delete;

    // IRenderInterface 实现
    // 着色器管理
    RenderResourceHandle createShader(ShaderType type, const char* source) override;
    RenderResourceHandle createProgram(const std::queue<RenderResourceHandle>& shaderQueue) override;
    void deleteShader(RenderResourceHandle shader) override;
    void deleteProgram(RenderResourceHandle program) override;
    void useProgram(RenderResourceHandle program) override;
    int32_t getUniformLocation(RenderResourceHandle program, const char* name) override;
    int32_t getAttribLocation(RenderResourceHandle program, const char* name) override;
    int32_t getActiveAttribCount(RenderResourceHandle program) override;
    int32_t getActiveUniformCount(RenderResourceHandle program) override;
    void getActiveAttrib(RenderResourceHandle program, uint32_t index, int32_t bufSize, int32_t* length, int32_t* size, DataType* type, char* name) override;
    void getActiveUniform(RenderResourceHandle program, uint32_t index, int32_t bufSize, int32_t* length, int32_t* size, DataType* type, char* name) override;

    // 缓冲区管理
    RenderResourceHandle createVertexBuffer(const void* data, size_t size, BufferUsage usage) override;
    RenderResourceHandle createIndexBuffer(const void* data, size_t size, BufferUsage usage) override;
    void deleteBuffer(RenderResourceHandle buffer) override;
    void bindVertexBuffer(RenderResourceHandle buffer) override;
    void bindIndexBuffer(RenderResourceHandle buffer) override;
    void updateBufferData(RenderResourceHandle buffer, size_t offset, size_t size, const void* data) override;

    // 顶点数组对象
    RenderResourceHandle createVertexArray() override;
    void deleteVertexArray(RenderResourceHandle vao) override;
    void bindVertexArray(RenderResourceHandle vao) override;
    void enableVertexAttrib(uint32_t index) override;
    void disableVertexAttrib(uint32_t index) override;
    void setVertexAttribPointer(uint32_t index, int32_t size, DataType type, bool normalized, uint32_t stride, const void* pointer) override;

    // 纹理管理
    RenderResourceHandle createTexture2D(int32_t width, int32_t height, TextureFormat format, const void* data) override;
    void deleteTexture(RenderResourceHandle texture) override;
    void bindTexture2D(RenderResourceHandle texture, uint32_t unit) override;
    void activeTextureUnit(uint32_t unit) override;
    uint32_t getActiveTextureUnit() override;
    void setTextureFilter(TextureFilter minFilter, TextureFilter magFilter) override;
    void setTextureWrap(TextureWrap wrapS, TextureWrap wrapT) override;
    void generateMipmap() override;
    void updateTexture2D(RenderResourceHandle texture, int32_t x, int32_t y, int32_t width, int32_t height, const void* data) override;

    // 帧缓冲
    RenderResourceHandle createFramebuffer() override;
    void deleteFramebuffer(RenderResourceHandle framebuffer) override;
    void bindFramebuffer(RenderResourceHandle framebuffer) override;
    void attachTextureToFramebuffer(RenderResourceHandle framebuffer, RenderResourceHandle texture, FramebufferAttachment attachment) override;
    bool checkFramebufferComplete(RenderResourceHandle framebuffer) override;

    // 渲染状态
    void setViewport(int32_t x, int32_t y, int32_t width, int32_t height) override;
    void setClearColor(float r, float g, float b, float a) override;
    void clear(bool color, bool depth, bool stencil) override;
    void enableDepthTest(bool enable) override;
    void enableBlend(bool enable) override;
    void setBlendFunc(BlendFactor src, BlendFactor dst) override;
    void enableCullFace(bool enable) override;
    void setCullFace(CullFace face) override;
    void setFrontFace(FrontFace face) override;

    // 绘制
    void drawArrays(DrawMode mode, int32_t first, int32_t count) override;
    void drawElements(DrawMode mode, int32_t count, DataType type, const void* indices) override;

    // 工具函数
    bool checkError(const char* operation) override;
    void finish() override;
    void flush() override;

    // Uniform 设置
    void setUniformInt(int32_t location, int32_t value) override;
    void setUniformFloat(int32_t location, float value) override;
    void setUniformVec2(int32_t location, float x, float y) override;
    void setUniformVec3(int32_t location, float x, float y, float z) override;
    void setUniformVec4(int32_t location, float x, float y, float z, float w) override;
    void setUniformMat4(int32_t location, const float* matrix, bool transpose) override;
private:

    // 内部类型转换辅助函数
    static GLenum toGLShaderType(ShaderType type);
    static GLenum toGLBufferUsage(BufferUsage usage);
    static GLenum toGLDataType(DataType type);
    static DataType fromGLDataType(GLenum type);
    static GLenum toGLTextureFormat(TextureFormat format);
    static GLenum toGLTextureInternalFormat(TextureFormat format);
    static GLenum toGLTextureFilter(TextureFilter filter);
    static GLenum toGLTextureWrap(TextureWrap wrap);
    static GLenum toGLBlendFactor(BlendFactor factor);
    static GLenum toGLCullFace(CullFace face);
    static GLenum toGLFrontFace(FrontFace face);
    static GLenum toGLDrawMode(DrawMode mode);
    static GLenum toGLFramebufferAttachment(FramebufferAttachment attachment);
};

#endif // OPENGLES_RENDER_INTERFACE_H
