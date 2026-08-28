//
// Created by banma-3361 on 2025/11/19.
//

#include "Shader.h"
#include "RenderInterface.h"
#include "Log.h"

namespace engine {
namespace renderer {


KEY_VALUE(TAG, Shader)

Shader::Shader(const std::string& name)
    : mName(name)
    , mProgram(INVALID_HANDLE)
{
}

Shader::~Shader() {
    // 删除程序
    if (mProgram != INVALID_HANDLE) {
        RenderInterface.deleteProgram(mProgram);
    }
    // 删除队列中残留的着色器（如果链接失败或未链接）
    while (!mShaderQueue.empty()) {
        RenderInterface.deleteShader(mShaderQueue.front());
        mShaderQueue.pop();
    }
}

bool Shader::compile(Type type, const char* source) {
    if (!source) {
        LogE("%s shader source is null", TAG);
        return false;
    }

    ShaderType shaderType = (type == Type::Vertex) ? ShaderType::Vertex : ShaderType::Fragment;
    RenderResourceHandle shader = RenderInterface.createShader(shaderType, source);

    if (shader == INVALID_HANDLE) {
        LogE("%s failed to compile %s shader", TAG, (type == Type::Vertex) ? "vertex" : "fragment");
        return false;
    }

    mShaderQueue.push(shader);

    LogI("%s compiled %s shader successfully", TAG, (type == Type::Vertex) ? "vertex" : "fragment");
    return true;
}

bool Shader::compile(Type type, const std::string& source) {
    return compile(type, source.c_str());
}

bool Shader::link() {
    if (mShaderQueue.size() < 2) {
        LogE("%s at least 2 shaders required for linking (vertex + fragment)", TAG);
        return false;
    }

    // 删除旧的程序
    if (mProgram != INVALID_HANDLE) {
        RenderInterface.deleteProgram(mProgram);
        mProgram = INVALID_HANDLE;
    }

    // 通过 RenderInterface 创建程序（直接传递队列）
    mProgram = RenderInterface.createProgram(mShaderQueue);

    // 链接成功后删除着色器（它们已经被附加到程序中）
    if (mProgram != INVALID_HANDLE) {
        while (!mShaderQueue.empty()) {
            RenderInterface.deleteShader(mShaderQueue.front());
            mShaderQueue.pop();
        }

        mLinked = true;

        // 反射获取属性和 Uniform
        reflectAttributes();
        reflectUniforms();

        LogI("%s linked program '%s' successfully", TAG, mName.c_str());
        return true;
    } else {
        LogE("%s failed to create shader program", TAG);
        mLinked = false;
        return false;
    }
}

void Shader::bind() const {
    if (mProgram != INVALID_HANDLE) {
        RenderInterface.useProgram(mProgram);
    }
}

void Shader::unbind() const {
    RenderInterface.useProgram(INVALID_HANDLE);
}

int32_t Shader::getAttributeLocation(const std::string& name) const {
    auto it = mAttributes.find(name);
    if (it != mAttributes.end()) {
        return it->second;
    }
    return -1;
}

int32_t Shader::getUniformLocation(const std::string& name) const {
    auto it = mUniforms.find(name);
    if (it != mUniforms.end()) {
        return it->second;
    }
    return -1;
}

void Shader::setUniformInt(const std::string& name, int32_t value) const {
    int32_t location = getUniformLocation(name);
    if (location >= 0) {
        RenderInterface.setUniformInt(location, value);
    }
}

void Shader::setUniformFloat(const std::string& name, float value) const {
    int32_t location = getUniformLocation(name);
    if (location >= 0) {
        RenderInterface.setUniformFloat(location, value);
    }
}

void Shader::setUniformVec2(const std::string& name, float x, float y) const {
    int32_t location = getUniformLocation(name);
    if (location >= 0) {
        RenderInterface.setUniformVec2(location, x, y);
    }
}

void Shader::setUniformVec3(const std::string& name, float x, float y, float z) const {
    int32_t location = getUniformLocation(name);
    if (location >= 0) {
        RenderInterface.setUniformVec3(location, x, y, z);
    }
}

void Shader::setUniformVec4(const std::string& name, float x, float y, float z, float w) const {
    int32_t location = getUniformLocation(name);
    if (location >= 0) {
        RenderInterface.setUniformVec4(location, x, y, z, w);
    }
}

void Shader::setUniformMat4(const std::string& name, const float* matrix, bool transpose) const {
    int32_t location = getUniformLocation(name);
    if (location >= 0) {
        RenderInterface.setUniformMat4(location, matrix, transpose);
    }
}

// GLM 便捷方法实现
void Shader::setUniformVec2(const std::string& name, const glm::vec2& v) const {
    setUniformVec2(name, v.x, v.y);
}

void Shader::setUniformVec3(const std::string& name, const glm::vec3& v) const {
    setUniformVec3(name, v.x, v.y, v.z);
}

void Shader::setUniformVec4(const std::string& name, const glm::vec4& v) const {
    setUniformVec4(name, v.x, v.y, v.z, v.w);
}

void Shader::setUniformMat4(const std::string& name, const glm::mat4& matrix, bool transpose) const {
    setUniformMat4(name, glm::value_ptr(matrix), transpose);
}

void Shader::reflectAttributes() {
    if (mProgram == INVALID_HANDLE) {
        return;
    }

    mAttributes.clear();

    int32_t count = RenderInterface.getActiveAttribCount(mProgram);
    constexpr int32_t NAME_BUF_SIZE = 256;
    char nameBuf[NAME_BUF_SIZE];

    for (int32_t i = 0; i < count; ++i) {
        int32_t length = 0, size = 0;
        DataType type;
        RenderInterface.getActiveAttrib(mProgram, i, NAME_BUF_SIZE, &length, &size, &type, nameBuf);

        int32_t location = RenderInterface.getAttribLocation(mProgram, nameBuf);
        if (location >= 0) {
            mAttributes.emplace(nameBuf, location);
            LogI("%s Attribute: %s = %d", TAG, nameBuf, location);
        }
    }
}

void Shader::reflectUniforms() {
    if (mProgram == INVALID_HANDLE) {
        return;
    }

    mUniforms.clear();

    int32_t count = RenderInterface.getActiveUniformCount(mProgram);
    constexpr int32_t NAME_BUF_SIZE = 256;
    char nameBuf[NAME_BUF_SIZE];

    for (int32_t i = 0; i < count; ++i) {
        int32_t length = 0, size = 0;
        DataType type;
        RenderInterface.getActiveUniform(mProgram, i, NAME_BUF_SIZE, &length, &size, &type, nameBuf);

        int32_t location = RenderInterface.getUniformLocation(mProgram, nameBuf);
        if (location >= 0) {
            mUniforms.emplace(nameBuf, location);
            LogI("%s Uniform: %s = %d", TAG, nameBuf, location);
        }
    }
}

} // namespace renderer
} // namespace engine
