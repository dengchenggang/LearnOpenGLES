#include "OpenGLESRenderInterface.h"
#include "utils/Log.h"

namespace engine {
namespace renderer {

KEY_VALUE(TAG, OpenGLESRenderInterface)

// 着色器管理
RenderResourceHandle OpenGLESRenderInterface::createShader(ShaderType type, const char* source) {
    GLuint shader = glCreateShader(toGLShaderType(type));
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint compiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        GLint infoLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen > 1) {
            char* infoLog = new char[infoLen];
            glGetShaderInfoLog(shader, infoLen, nullptr, infoLog);
            LogE("%s Shader compile error: %s", TAG, infoLog);
            delete[] infoLog;
        }
        glDeleteShader(shader);
        return INVALID_HANDLE;
    }
    return static_cast<RenderResourceHandle>(shader);
}

RenderResourceHandle OpenGLESRenderInterface::createProgram(const std::queue<RenderResourceHandle>& shaderQueue) {
    if (shaderQueue.empty()) {
        LogE("%s At least one shader required", TAG);
        return INVALID_HANDLE;
    }

    GLuint program = glCreateProgram();

    // 附加所有着色器（从队列复制遍历）
    std::queue<RenderResourceHandle> tempQueue = shaderQueue;
    while (!tempQueue.empty()) {
        GLuint glShader = static_cast<GLuint>(tempQueue.front());
        if (glShader == 0) {
            LogE("%s Invalid shader handle", TAG);
            glDeleteProgram(program);
            return INVALID_HANDLE;
        }
        glAttachShader(program, glShader);
        tempQueue.pop();
    }

    glLinkProgram(program);

    GLint linked;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked) {
        GLint infoLen = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen > 1) {
            char* infoLog = new char[infoLen];
            glGetProgramInfoLog(program, infoLen, nullptr, infoLog);
            LogE("%s Program link error: %s", TAG, infoLog);
            delete[] infoLog;
        }
        glDeleteProgram(program);
        program = 0;
    }

    return static_cast<RenderResourceHandle>(program);
}

void OpenGLESRenderInterface::deleteShader(RenderResourceHandle shader) {
    glDeleteShader(static_cast<GLuint>(shader));
}

void OpenGLESRenderInterface::deleteProgram(RenderResourceHandle program) {
    glDeleteProgram(static_cast<GLuint>(program));
}

void OpenGLESRenderInterface::useProgram(RenderResourceHandle program) {
    glUseProgram(static_cast<GLuint>(program));
}

int32_t OpenGLESRenderInterface::getUniformLocation(RenderResourceHandle program, const char* name) {
    return glGetUniformLocation(static_cast<GLuint>(program), name);
}

int32_t OpenGLESRenderInterface::getAttribLocation(RenderResourceHandle program, const char* name) {
    return glGetAttribLocation(static_cast<GLuint>(program), name);
}

int32_t OpenGLESRenderInterface::getActiveAttribCount(RenderResourceHandle program) {
    GLint count = 0;
    glGetProgramiv(static_cast<GLuint>(program), GL_ACTIVE_ATTRIBUTES, &count);
    return count;
}

int32_t OpenGLESRenderInterface::getActiveUniformCount(RenderResourceHandle program) {
    GLint count = 0;
    glGetProgramiv(static_cast<GLuint>(program), GL_ACTIVE_UNIFORMS, &count);
    return count;
}

void OpenGLESRenderInterface::getActiveAttrib(RenderResourceHandle program, uint32_t index, int32_t bufSize, int32_t* length, int32_t* size, DataType* type, char* name) {
    GLenum glType;
    glGetActiveAttrib(static_cast<GLuint>(program), index, bufSize,
                      reinterpret_cast<GLsizei*>(length),
                      reinterpret_cast<GLint*>(size),
                      &glType, name);
    if (type) {
        *type = fromGLDataType(glType);
    }
}

void OpenGLESRenderInterface::getActiveUniform(RenderResourceHandle program, uint32_t index, int32_t bufSize, int32_t* length, int32_t* size, DataType* type, char* name) {
    GLenum glType;
    glGetActiveUniform(static_cast<GLuint>(program), index, bufSize,
                       reinterpret_cast<GLsizei*>(length),
                       reinterpret_cast<GLint*>(size),
                       &glType, name);
    if (type) {
        *type = fromGLDataType(glType);
    }
}

// 缓冲区管理
RenderResourceHandle OpenGLESRenderInterface::createVertexBuffer(const void* data, size_t size, BufferUsage usage) {
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, size, data, toGLBufferUsage(usage));
    return static_cast<RenderResourceHandle>(buffer);
}

RenderResourceHandle OpenGLESRenderInterface::createIndexBuffer(const void* data, size_t size, BufferUsage usage) {
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, toGLBufferUsage(usage));
    return static_cast<RenderResourceHandle>(buffer);
}

void OpenGLESRenderInterface::deleteBuffer(RenderResourceHandle buffer) {
    GLuint glBuffer = static_cast<GLuint>(buffer);
    glDeleteBuffers(1, &glBuffer);
}

void OpenGLESRenderInterface::bindVertexBuffer(RenderResourceHandle buffer) {
    glBindBuffer(GL_ARRAY_BUFFER, static_cast<GLuint>(buffer));
}

void OpenGLESRenderInterface::bindIndexBuffer(RenderResourceHandle buffer) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLuint>(buffer));
}

void OpenGLESRenderInterface::updateBufferData(RenderResourceHandle buffer, size_t offset, size_t size, const void* data) {
    glBindBuffer(GL_ARRAY_BUFFER, static_cast<GLuint>(buffer));
    glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}

// 顶点数组对象
RenderResourceHandle OpenGLESRenderInterface::createVertexArray() {
    GLuint vao;
    glGenVertexArrays(1, &vao);
    return static_cast<RenderResourceHandle>(vao);
}

void OpenGLESRenderInterface::deleteVertexArray(RenderResourceHandle vao) {
    GLuint glVao = static_cast<GLuint>(vao);
    glDeleteVertexArrays(1, &glVao);
}

void OpenGLESRenderInterface::bindVertexArray(RenderResourceHandle vao) {
    glBindVertexArray(static_cast<GLuint>(vao));
}

void OpenGLESRenderInterface::enableVertexAttrib(uint32_t index) {
    glEnableVertexAttribArray(index);
}

void OpenGLESRenderInterface::disableVertexAttrib(uint32_t index) {
    glDisableVertexAttribArray(index);
}

void OpenGLESRenderInterface::setVertexAttribPointer(uint32_t index, int32_t size, DataType type, bool normalized, uint32_t stride, const void* pointer) {
    glVertexAttribPointer(index, size, toGLDataType(type), normalized, stride, pointer);
}

// 纹理管理
RenderResourceHandle OpenGLESRenderInterface::createTexture2D(int32_t width, int32_t height, TextureFormat format, const void* data) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, toGLTextureInternalFormat(format), width, height, 0, toGLTextureFormat(format), GL_UNSIGNED_BYTE, data);
    return static_cast<RenderResourceHandle>(texture);
}

void OpenGLESRenderInterface::deleteTexture(RenderResourceHandle texture) {
    GLuint glTexture = static_cast<GLuint>(texture);
    glDeleteTextures(1, &glTexture);
}

void OpenGLESRenderInterface::bindTexture2D(RenderResourceHandle texture, uint32_t unit) {
    activeTextureUnit(unit);
    glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(texture));
}

void OpenGLESRenderInterface::activeTextureUnit(uint32_t unit) {
    glActiveTexture(GL_TEXTURE0 + unit);
}

uint32_t OpenGLESRenderInterface::getActiveTextureUnit() {
    GLint activeUnit;
    glGetIntegerv(GL_ACTIVE_TEXTURE, &activeUnit);
    return static_cast<uint32_t>(activeUnit - GL_TEXTURE0);
}

void OpenGLESRenderInterface::setTextureFilter(TextureFilter minFilter, TextureFilter magFilter) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, toGLTextureFilter(minFilter));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, toGLTextureFilter(magFilter));
}

void OpenGLESRenderInterface::setTextureWrap(TextureWrap wrapS, TextureWrap wrapT) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, toGLTextureWrap(wrapS));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, toGLTextureWrap(wrapT));
}

void OpenGLESRenderInterface::generateMipmap() {
    glGenerateMipmap(GL_TEXTURE_2D);
}

void OpenGLESRenderInterface::updateTexture2D(RenderResourceHandle texture, int32_t x, int32_t y, int32_t width, int32_t height, const void* data) {
    if (texture == INVALID_HANDLE || !data || width <= 0 || height <= 0) {
        return;
    }

    // 保存当前绑定的纹理
    GLint prevTexture;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &prevTexture);

    // 绑定目标纹理并更新数据
    glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(texture));
    glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);

    // 恢复之前的绑定
    glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(prevTexture));
}

// 帧缓冲
RenderResourceHandle OpenGLESRenderInterface::createFramebuffer() {
    GLuint framebuffer;
    glGenFramebuffers(1, &framebuffer);
    return static_cast<RenderResourceHandle>(framebuffer);
}

void OpenGLESRenderInterface::deleteFramebuffer(RenderResourceHandle framebuffer) {
    GLuint glFb = static_cast<GLuint>(framebuffer);
    glDeleteFramebuffers(1, &glFb);
}

void OpenGLESRenderInterface::bindFramebuffer(RenderResourceHandle framebuffer) {
    glBindFramebuffer(GL_FRAMEBUFFER, static_cast<GLuint>(framebuffer));
}

void OpenGLESRenderInterface::attachTextureToFramebuffer(RenderResourceHandle framebuffer, RenderResourceHandle texture, FramebufferAttachment attachment) {
    glBindFramebuffer(GL_FRAMEBUFFER, static_cast<GLuint>(framebuffer));
    glFramebufferTexture2D(GL_FRAMEBUFFER, toGLFramebufferAttachment(attachment), GL_TEXTURE_2D, static_cast<GLuint>(texture), 0);
}

bool OpenGLESRenderInterface::checkFramebufferComplete(RenderResourceHandle framebuffer) {
    bindFramebuffer(framebuffer);
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    return status == GL_FRAMEBUFFER_COMPLETE;
}

// 渲染状态
void OpenGLESRenderInterface::setViewport(int32_t x, int32_t y, int32_t width, int32_t height) {
    glViewport(x, y, width, height);
}

void OpenGLESRenderInterface::getViewport(int32_t* x, int32_t* y, int32_t* width, int32_t* height) {
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    if (x) *x = viewport[0];
    if (y) *y = viewport[1];
    if (width) *width = viewport[2];
    if (height) *height = viewport[3];
}

void OpenGLESRenderInterface::setClearColor(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
}

void OpenGLESRenderInterface::clear(bool color, bool depth, bool stencil) {
    GLbitfield mask = 0;
    if (color) mask |= GL_COLOR_BUFFER_BIT;
    if (depth) mask |= GL_DEPTH_BUFFER_BIT;
    if (stencil) mask |= GL_STENCIL_BUFFER_BIT;
    glClear(mask);
}

void OpenGLESRenderInterface::enableDepthTest(bool enable) {
    if (enable) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }
}

void OpenGLESRenderInterface::enableBlend(bool enable) {
    if (enable) {
        glEnable(GL_BLEND);
    } else {
        glDisable(GL_BLEND);
    }
}

void OpenGLESRenderInterface::setBlendFunc(BlendFactor src, BlendFactor dst) {
    glBlendFunc(toGLBlendFactor(src), toGLBlendFactor(dst));
}

void OpenGLESRenderInterface::enableCullFace(bool enable) {
    if (enable) {
        glEnable(GL_CULL_FACE);
    } else {
        glDisable(GL_CULL_FACE);
    }
}

void OpenGLESRenderInterface::setCullFace(CullFace face) {
    glCullFace(toGLCullFace(face));
}

void OpenGLESRenderInterface::setFrontFace(FrontFace face) {
    glFrontFace(toGLFrontFace(face));
}

// 绘制
void OpenGLESRenderInterface::drawArrays(DrawMode mode, int32_t first, int32_t count) {
    glDrawArrays(toGLDrawMode(mode), first, count);
}

void OpenGLESRenderInterface::drawElements(DrawMode mode, int32_t count, DataType type, const void* indices) {
    glDrawElements(toGLDrawMode(mode), count, toGLDataType(type), indices);
}

// 工具函数
bool OpenGLESRenderInterface::checkError(const char* operation) {
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        const char* errorStr = "UNKNOWN";
        switch (error) {
            case GL_NO_ERROR: errorStr = "GL_NO_ERROR"; break;
            case GL_INVALID_ENUM: errorStr = "GL_INVALID_ENUM"; break;
            case GL_INVALID_VALUE: errorStr = "GL_INVALID_VALUE"; break;
            case GL_INVALID_OPERATION: errorStr = "GL_INVALID_OPERATION"; break;
            case GL_OUT_OF_MEMORY: errorStr = "GL_OUT_OF_MEMORY"; break;
        }
        LogE("%s OpenGL error in %s: %s", TAG, operation, errorStr);
        return false;
    }
    return true;
}

void OpenGLESRenderInterface::finish() {
    glFinish();
}

void OpenGLESRenderInterface::flush() {
    glFlush();
}

// Uniform 设置
void OpenGLESRenderInterface::setUniformInt(int32_t location, int32_t value) {
    glUniform1i(location, value);
}

void OpenGLESRenderInterface::setUniformFloat(int32_t location, float value) {
    glUniform1f(location, value);
}

void OpenGLESRenderInterface::setUniformVec2(int32_t location, float x, float y) {
    glUniform2f(location, x, y);
}

void OpenGLESRenderInterface::setUniformVec3(int32_t location, float x, float y, float z) {
    glUniform3f(location, x, y, z);
}

void OpenGLESRenderInterface::setUniformVec4(int32_t location, float x, float y, float z, float w) {
    glUniform4f(location, x, y, z, w);
}

void OpenGLESRenderInterface::setUniformMat4(int32_t location, const float* matrix, bool transpose) {
    glUniformMatrix4fv(location, 1, transpose ? GL_TRUE : GL_FALSE, matrix);
}

// 类型转换辅助函数
GLenum OpenGLESRenderInterface::toGLShaderType(ShaderType type) {
    switch (type) {
        case ShaderType::Vertex: return GL_VERTEX_SHADER;
        case ShaderType::Fragment: return GL_FRAGMENT_SHADER;
    }
    return GL_VERTEX_SHADER;
}

GLenum OpenGLESRenderInterface::toGLBufferUsage(BufferUsage usage) {
    switch (usage) {
        case BufferUsage::Static: return GL_STATIC_DRAW;
        case BufferUsage::Dynamic: return GL_DYNAMIC_DRAW;
        case BufferUsage::Stream: return GL_STREAM_DRAW;
    }
    return GL_STATIC_DRAW;
}

GLenum OpenGLESRenderInterface::toGLDataType(DataType type) {
    switch (type) {
        case DataType::Byte: return GL_BYTE;
        case DataType::UByte: return GL_UNSIGNED_BYTE;
        case DataType::Short: return GL_SHORT;
        case DataType::UShort: return GL_UNSIGNED_SHORT;
        case DataType::Int: return GL_INT;
        case DataType::UInt: return GL_UNSIGNED_INT;
        case DataType::Float: return GL_FLOAT;
    }
    return GL_FLOAT;
}

DataType OpenGLESRenderInterface::fromGLDataType(GLenum type) {
    switch (type) {
        case GL_BYTE: return DataType::Byte;
        case GL_UNSIGNED_BYTE: return DataType::UByte;
        case GL_SHORT: return DataType::Short;
        case GL_UNSIGNED_SHORT: return DataType::UShort;
        case GL_INT: return DataType::Int;
        case GL_UNSIGNED_INT: return DataType::UInt;
        case GL_FLOAT: return DataType::Float;
    }
    return DataType::Float;
}

GLenum OpenGLESRenderInterface::toGLTextureFormat(TextureFormat format) {
    switch (format) {
        case TextureFormat::RGB8:
        case TextureFormat::RGB565: return GL_RGB;
        case TextureFormat::RGBA8:
        case TextureFormat::RGBA4444: return GL_RGBA;
        case TextureFormat::Depth16:
        case TextureFormat::Depth24: return GL_DEPTH_COMPONENT;
        case TextureFormat::Depth24Stencil8: return GL_DEPTH_STENCIL;
    }
    return GL_RGBA;
}

GLenum OpenGLESRenderInterface::toGLTextureInternalFormat(TextureFormat format) {
    switch (format) {
        case TextureFormat::RGB8: return GL_RGB8;
        case TextureFormat::RGBA8: return GL_RGBA8;
        case TextureFormat::RGB565: return GL_RGB565;
        case TextureFormat::RGBA4444: return GL_RGBA4;
        case TextureFormat::Depth16: return GL_DEPTH_COMPONENT16;
        case TextureFormat::Depth24: return GL_DEPTH_COMPONENT24;
        case TextureFormat::Depth24Stencil8: return GL_DEPTH24_STENCIL8;
    }
    return GL_RGBA8;
}

GLenum OpenGLESRenderInterface::toGLTextureFilter(TextureFilter filter) {
    switch (filter) {
        case TextureFilter::Nearest: return GL_NEAREST;
        case TextureFilter::Linear: return GL_LINEAR;
        case TextureFilter::NearestMipmapNearest: return GL_NEAREST_MIPMAP_NEAREST;
        case TextureFilter::LinearMipmapNearest: return GL_LINEAR_MIPMAP_NEAREST;
        case TextureFilter::NearestMipmapLinear: return GL_NEAREST_MIPMAP_LINEAR;
        case TextureFilter::LinearMipmapLinear: return GL_LINEAR_MIPMAP_LINEAR;
    }
    return GL_LINEAR;
}

GLenum OpenGLESRenderInterface::toGLTextureWrap(TextureWrap wrap) {
    switch (wrap) {
        case TextureWrap::Repeat: return GL_REPEAT;
        case TextureWrap::ClampToEdge: return GL_CLAMP_TO_EDGE;
        case TextureWrap::MirroredRepeat: return GL_MIRRORED_REPEAT;
    }
    return GL_CLAMP_TO_EDGE;
}

GLenum OpenGLESRenderInterface::toGLBlendFactor(BlendFactor factor) {
    switch (factor) {
        case BlendFactor::Zero: return GL_ZERO;
        case BlendFactor::One: return GL_ONE;
        case BlendFactor::SrcColor: return GL_SRC_COLOR;
        case BlendFactor::OneMinusSrcColor: return GL_ONE_MINUS_SRC_COLOR;
        case BlendFactor::SrcAlpha: return GL_SRC_ALPHA;
        case BlendFactor::OneMinusSrcAlpha: return GL_ONE_MINUS_SRC_ALPHA;
        case BlendFactor::DstAlpha: return GL_DST_ALPHA;
        case BlendFactor::OneMinusDstAlpha: return GL_ONE_MINUS_DST_ALPHA;
    }
    return GL_ONE;
}

GLenum OpenGLESRenderInterface::toGLCullFace(CullFace face) {
    switch (face) {
        case CullFace::Front: return GL_FRONT;
        case CullFace::Back: return GL_BACK;
        case CullFace::FrontAndBack: return GL_FRONT_AND_BACK;
    }
    return GL_BACK;
}

GLenum OpenGLESRenderInterface::toGLFrontFace(FrontFace face) {
    switch (face) {
        case FrontFace::Clockwise: return GL_CW;
        case FrontFace::CounterClockwise: return GL_CCW;
    }
    return GL_CCW;
}

GLenum OpenGLESRenderInterface::toGLDrawMode(DrawMode mode) {
    switch (mode) {
        case DrawMode::Points: return GL_POINTS;
        case DrawMode::Lines: return GL_LINES;
        case DrawMode::LineLoop: return GL_LINE_LOOP;
        case DrawMode::LineStrip: return GL_LINE_STRIP;
        case DrawMode::Triangles: return GL_TRIANGLES;
        case DrawMode::TriangleStrip: return GL_TRIANGLE_STRIP;
        case DrawMode::TriangleFan: return GL_TRIANGLE_FAN;
    }
    return GL_TRIANGLES;
}

GLenum OpenGLESRenderInterface::toGLFramebufferAttachment(FramebufferAttachment attachment) {
    switch (attachment) {
        case FramebufferAttachment::Color0: return GL_COLOR_ATTACHMENT0;
        case FramebufferAttachment::Color1: return GL_COLOR_ATTACHMENT1;
        case FramebufferAttachment::Color2: return GL_COLOR_ATTACHMENT2;
        case FramebufferAttachment::Color3: return GL_COLOR_ATTACHMENT3;
        case FramebufferAttachment::Depth: return GL_DEPTH_ATTACHMENT;
        case FramebufferAttachment::Stencil: return GL_STENCIL_ATTACHMENT;
        case FramebufferAttachment::DepthStencil: return GL_DEPTH_STENCIL_ATTACHMENT;
    }
    return GL_COLOR_ATTACHMENT0;
}

} // namespace renderer
} // namespace engine
