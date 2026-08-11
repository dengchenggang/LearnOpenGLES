#ifndef I_RENDER_INTERFACE_H
#define I_RENDER_INTERFACE_H

#include <cstddef>
#include <cstdint>
#include <queue>
#include "RenderInterfaceDef.h"

namespace engine {
namespace renderer {

class IRenderInterface {
public:
    IRenderInterface() = default;
    virtual ~IRenderInterface() = default;
    IRenderInterface(const IRenderInterface&) = delete;
    IRenderInterface& operator = (const IRenderInterface&) = delete;

    // 着色器管理
    virtual RenderResourceHandle createShader(ShaderType type, const char* source) = 0;

    // 创建程序，支持队列接口（基础接口）
    virtual RenderResourceHandle createProgram(const std::queue<RenderResourceHandle>& shaderQueue) = 0;

    // 创建程序，支持参数列表（便捷接口，内部调用队列接口）
    RenderResourceHandle createProgram(std::initializer_list<RenderResourceHandle> shaders) {
        std::queue<RenderResourceHandle> queue;
        for (auto shader : shaders) {
            queue.push(shader);
        }
        return createProgram(queue);
    }
    virtual void deleteShader(RenderResourceHandle shader) = 0;
    virtual void deleteProgram(RenderResourceHandle program) = 0;
    virtual void useProgram(RenderResourceHandle program) = 0;
    virtual int32_t getUniformLocation(RenderResourceHandle program, const char* name) = 0;
    virtual int32_t getAttribLocation(RenderResourceHandle program, const char* name) = 0;

    // 获取 active attribute 数量
    virtual int32_t getActiveAttribCount(RenderResourceHandle program) = 0;
    // 获取 active uniform 数量
    virtual int32_t getActiveUniformCount(RenderResourceHandle program) = 0;
    // 获取指定索引的 attribute 信息
    virtual void getActiveAttrib(RenderResourceHandle program, uint32_t index, int32_t bufSize, int32_t* length, int32_t* size, DataType* type, char* name) = 0;
    // 获取指定索引的 uniform 信息
    virtual void getActiveUniform(RenderResourceHandle program, uint32_t index, int32_t bufSize, int32_t* length, int32_t* size, DataType* type, char* name) = 0;

    // 缓冲区管理
    virtual RenderResourceHandle createVertexBuffer(const void* data, size_t size, BufferUsage usage) = 0;
    virtual RenderResourceHandle createIndexBuffer(const void* data, size_t size, BufferUsage usage) = 0;
    virtual void deleteBuffer(RenderResourceHandle buffer) = 0;
    virtual void bindVertexBuffer(RenderResourceHandle buffer) = 0;
    virtual void bindIndexBuffer(RenderResourceHandle buffer) = 0;
    virtual void updateBufferData(RenderResourceHandle buffer, size_t offset, size_t size, const void* data) = 0;

    // 顶点数组对象
    virtual RenderResourceHandle createVertexArray() = 0;
    virtual void deleteVertexArray(RenderResourceHandle vao) = 0;
    virtual void bindVertexArray(RenderResourceHandle vao) = 0;
    virtual void enableVertexAttrib(uint32_t index) = 0;
    virtual void disableVertexAttrib(uint32_t index) = 0;
    virtual void setVertexAttribPointer(uint32_t index, int32_t size, DataType type, bool normalized, uint32_t stride, const void* pointer) = 0;

    // 纹理管理
    virtual RenderResourceHandle createTexture2D(int32_t width, int32_t height, TextureFormat format, const void* data) = 0;
    virtual void deleteTexture(RenderResourceHandle texture) = 0;
    virtual void bindTexture2D(RenderResourceHandle texture, uint32_t unit) = 0;
    virtual void activeTextureUnit(uint32_t unit) = 0;
    virtual uint32_t getActiveTextureUnit() = 0;
    virtual void setTextureFilter(TextureFilter minFilter, TextureFilter magFilter) = 0;
    virtual void setTextureWrap(TextureWrap wrapS, TextureWrap wrapT) = 0;
    virtual void generateMipmap() = 0;
    virtual void updateTexture2D(RenderResourceHandle texture, int32_t x, int32_t y, int32_t width, int32_t height, const void* data) = 0;

    // 帧缓冲
    virtual RenderResourceHandle createFramebuffer() = 0;
    virtual void deleteFramebuffer(RenderResourceHandle framebuffer) = 0;
    virtual void bindFramebuffer(RenderResourceHandle framebuffer) = 0;
    virtual void attachTextureToFramebuffer(RenderResourceHandle framebuffer, RenderResourceHandle texture, FramebufferAttachment attachment) = 0;
    virtual bool checkFramebufferComplete(RenderResourceHandle framebuffer) = 0;

    // 渲染状态
    virtual void setViewport(int32_t x, int32_t y, int32_t width, int32_t height) = 0;
    virtual void getViewport(int32_t* x, int32_t* y, int32_t* width, int32_t* height) = 0;
    virtual void setClearColor(float r, float g, float b, float a) = 0;
    virtual void clear(bool color, bool depth, bool stencil) = 0;
    virtual void enableDepthTest(bool enable) = 0;
    virtual void enableBlend(bool enable) = 0;
    virtual void setBlendFunc(BlendFactor src, BlendFactor dst) = 0;
    virtual void enableCullFace(bool enable) = 0;
    virtual void setCullFace(CullFace face) = 0;
    virtual void setFrontFace(FrontFace face) = 0;

    // 绘制
    virtual void drawArrays(DrawMode mode, int32_t first, int32_t count) = 0;
    virtual void drawElements(DrawMode mode, int32_t count, DataType type, const void* indices) = 0;

    // 工具函数
    virtual bool checkError(const char* operation) = 0;
    virtual void finish() = 0;
    virtual void flush() = 0;

    // Uniform 设置
    virtual void setUniformInt(int32_t location, int32_t value) = 0;
    virtual void setUniformFloat(int32_t location, float value) = 0;
    virtual void setUniformVec2(int32_t location, float x, float y) = 0;
    virtual void setUniformVec3(int32_t location, float x, float y, float z) = 0;
    virtual void setUniformVec4(int32_t location, float x, float y, float z, float w) = 0;
    virtual void setUniformMat4(int32_t location, const float* matrix, bool transpose = false) = 0;
};

} // namespace renderer
} // namespace engine

#endif // I_RENDER_INTERFACE_H
