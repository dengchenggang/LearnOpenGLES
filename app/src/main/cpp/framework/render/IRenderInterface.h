#ifndef I_RENDER_INTERFACE_H
#define I_RENDER_INTERFACE_H

#include <cstddef>
#include <cstdint>
#include <queue>

// 渲染资源句柄（底层实现隐藏）
using RenderResourceHandle = uint32_t;
constexpr RenderResourceHandle INVALID_HANDLE = 0;

// 着色器类型
enum class ShaderType : uint8_t {
    Vertex,
    Fragment
};

// 缓冲区用途
enum class BufferUsage : uint8_t {
    Static,     // 数据不经常改变
    Dynamic,    // 数据经常改变
    Stream      // 每帧都改变
};

// 数据类型
enum class DataType : uint8_t {
    Byte,
    UByte,
    Short,
    UShort,
    Int,
    UInt,
    Float
};

// 纹理格式
enum class TextureFormat : uint8_t {
    RGB8,
    RGBA8,
    RGB565,
    RGBA4444,
    Depth16,
    Depth24,
    Depth24Stencil8
};

// 纹理过滤
enum class TextureFilter : uint8_t {
    Nearest,
    Linear,
    NearestMipmapNearest,
    LinearMipmapNearest,
    NearestMipmapLinear,
    LinearMipmapLinear
};

// 纹理环绕
enum class TextureWrap : uint8_t {
    Repeat,
    ClampToEdge,
    MirroredRepeat
};

// 渲染状态
enum class BlendFactor : uint8_t {
    Zero,
    One,
    SrcColor,
    OneMinusSrcColor,
    SrcAlpha,
    OneMinusSrcAlpha,
    DstAlpha,
    OneMinusDstAlpha
};

enum class CullFace : uint8_t {
    Front,
    Back,
    FrontAndBack
};

enum class FrontFace : uint8_t {
    Clockwise,
    CounterClockwise
};

// 绘制模式
enum class DrawMode : uint8_t {
    Points,
    Lines,
    LineLoop,
    LineStrip,
    Triangles,
    TriangleStrip,
    TriangleFan
};

// 附件类型
enum class FramebufferAttachment : uint8_t {
    Color0,
    Color1,
    Color2,
    Color3,
    Depth,
    Stencil,
    DepthStencil
};

// 统一变量类型
enum class UniformType : uint8_t {
    Int,
    Float,
    Vec2,
    Vec3,
    Vec4,
    Mat2,
    Mat3,
    Mat4
};

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

#endif // I_RENDER_INTERFACE_H
