//
// Created by banma-3361 on 2025/11/19.
//

#include "CShader.h"
#include <GLES3/gl3.h>
#include "CLogUtils.h"
#include <cstdio>
#include "bind_guard.hpp"


namespace engine {
KEY_VALUE(TAG, CShader)

constexpr std::uint32_t SHADER_INFO_LOG_BUFFER_SIZE = 512;
constexpr std::uint32_t ATTRIBUTE_NAME_BUFFER_SIZE  = 128;
constexpr std::uint32_t UNIFORM_NAME_BUFFER_SIZE    = 256;


static char gShaderLogInfoBuffer[SHADER_INFO_LOG_BUFFER_SIZE];
static char gAttributeNameBuffer[ATTRIBUTE_NAME_BUFFER_SIZE];
static char gUniformNameBuffer  [UNIFORM_NAME_BUFFER_SIZE];

CShader::CShader(std::string&& name)
    : mName(std::move(name))
{
    mProgram = glCreateProgram();
}

CShader::CShader(const std::string& name)
    : mName(name)
{
    mProgram = glCreateProgram();
}

CShader::~CShader() {
    glDeleteProgram(mProgram);
}

void CShader::bind() const {
    glUseProgram(mProgram);
}

void CShader::unbind() const {
    glUseProgram(0);
}

bool CShader::compile(std::uint32_t shaderType, const char* code, shader_container_type & shaders) const {
    bool ret {false};

    std::uint32_t shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &code, NULL);
    glCompileShader(shader);
    GLint sucess {GL_FALSE};
    glGetShaderiv(shader, GL_COMPILE_STATUS, &sucess);
    if (sucess == GL_TRUE) {
        shaders.emplace_back(shader_type {shaderType, shader});
        ret = true;
    } else {
        std::memset(gShaderLogInfoBuffer, 0, SHADER_INFO_LOG_BUFFER_SIZE);
        glGetShaderInfoLog(shader, SHADER_INFO_LOG_BUFFER_SIZE, NULL, gShaderLogInfoBuffer);
        glDeleteShader(shader);
    }

    return ret;
}

bool CShader::link(shader_container_type& shaders) {
    bool ret {false};

    for (const auto& shader : shaders) {
        glAttachShader(mProgram, shader.second);
    }

    glLinkProgram(mProgram);
    GLint success {GL_FALSE};
    glGetProgramiv(mProgram, GL_LINK_STATUS, &success);
    if (success == GL_TRUE) {
        ret = true;
    } else {
        std::memset(gShaderLogInfoBuffer, 0, SHADER_INFO_LOG_BUFFER_SIZE);
        glGetProgramInfoLog(mProgram, SHADER_INFO_LOG_BUFFER_SIZE, NULL, gShaderLogInfoBuffer);
    }

    for (const auto& shader : shaders) {
        glDeleteShader(shader.second);
    }
    shaders.clear();

    return ret;
}

void CShader::populateAttributes() {
    std::int32_t count {-1}, length {0}, size {0};
    GLenum attributeType;
    utils::bind_guard<CShader> binder {*this};
    glGetProgramiv(mProgram, GL_ACTIVE_ATTRIBUTES, &count);
    for (std::int32_t i = 0; i < count; ++i) {
        std::memset(gAttributeNameBuffer, 0, ATTRIBUTE_NAME_BUFFER_SIZE);
        glGetActiveAttrib(mProgram, static_cast<std::uint32_t>(i), ATTRIBUTE_NAME_BUFFER_SIZE, &length, &size, &attributeType, gAttributeNameBuffer);
        std::int32_t location {glGetAttribLocation(mProgram, gAttributeNameBuffer)};
        if (location >= 0) {
            mAttributes.emplace(gAttributeNameBuffer, location);
            LogI("%s populateAttributes: i=%u, %s=%d", TAG, i, gAttributeNameBuffer, location);
        } else {
            LogE("%s populateAttributes: i=%u, %s glGetAttribLocation failed!", TAG, i, gAttributeNameBuffer);
        }
    }
}

void CShader::populateUniforms() {
    std::int32_t count {-1}, length {0}, size {0};
    GLenum uniformType;
    utils::bind_guard<CShader> binder {*this};
    glGetProgramiv(mProgram, GL_ACTIVE_UNIFORMS, &count);
    for (std::int32_t i = 0; i < count; ++i) {
        std::memset(gAttributeNameBuffer, 0, ATTRIBUTE_NAME_BUFFER_SIZE);
        glGetActiveUniform(mProgram, static_cast<std::uint32_t>(i), ATTRIBUTE_NAME_BUFFER_SIZE, &length, &size, &uniformType, gAttributeNameBuffer);
        std::int32_t location {glGetUniformLocation(mProgram, gAttributeNameBuffer)};
        if (location >= 0) {
            std::string uniformName {gAttributeNameBuffer};
            auto pos = uniformName.find('[');
            if (pos != std::string::npos) {
                const std::string uniformNamePrefix {uniformName.begin(), uniformName.end()};
                std::uint32_t uniformIndex {0};
                while (true) {
                    std::memset(gUniformNameBuffer, 0, UNIFORM_NAME_BUFFER_SIZE);
                    std::snprintf(gUniformNameBuffer, UNIFORM_NAME_BUFFER_SIZE, "%s[%d]", uniformNamePrefix.c_str(), uniformIndex++);
                    std::int32_t uniformLocation {glGetUniformLocation(mProgram, gUniformNameBuffer)};
                    if (uniformLocation < 0) {
                        break;
                    }
                    mUniforms.emplace(gUniformNameBuffer, uniformLocation);
                    LogI("%s populateUniforms: i=%d, %s=%d", TAG, i, gUniformNameBuffer, uniformLocation);
                }
            } else {
                mUniforms.emplace(std::move(uniformName), location);
                LogI("%s populateUniforms: i=%d, %s=%d", TAG, i, gAttributeNameBuffer, location);
            }
        } else {
            LogE("%s populateUniforms: i=%d, name=%s glGetUniformLocation failed!", TAG, i, gAttributeNameBuffer);
        }
    }
}

}
