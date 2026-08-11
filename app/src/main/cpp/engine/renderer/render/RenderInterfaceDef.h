#ifndef RENDER_INTERFACE_DEF_H
#define RENDER_INTERFACE_DEF_H

#include <cstdint>

namespace engine {
namespace renderer {

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

} // namespace renderer
} // namespace engine

#endif // RENDER_INTERFACE_DEF_H
