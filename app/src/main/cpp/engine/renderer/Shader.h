//
// Created by banma-3361 on 2025/11/19.
//

#ifndef SHADER_H
#define SHADER_H

#include "IRenderInterface.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <cstdint>
#include <unordered_map>
#include <queue>

class Shader {
public:
    // 着色器类型
    enum class Type : uint8_t {
        Vertex,     // 顶点着色器
        Fragment    // 片段着色器
    };

private:
    std::string mName;
    RenderResourceHandle mProgram;       // 着色器程序句柄
    bool mLinked = false;

    // 缓存的属性和 Uniform 位置
    std::unordered_map<std::string, int32_t> mAttributes;
    std::unordered_map<std::string, int32_t> mUniforms;

    // 缓存着色器句柄队列（用于链接时创建程序）
    std::queue<RenderResourceHandle> mShaderQueue;

public:
    // 构造函数
    explicit Shader(const std::string& name);
    ~Shader();

    // 禁止拷贝和移动
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    // 编译着色器代码
    bool compile(Type type, const char* source);
    bool compile(Type type, const std::string& source);

    // 链接着色器程序
    bool link();

    // 使用着色器程序
    void bind() const;
    void unbind() const;

    // 获取属性和 Uniform 位置
    int32_t getAttributeLocation(const std::string& name) const;
    int32_t getUniformLocation(const std::string& name) const;

    // 设置 Uniform（基础方法）
    void setUniformInt(const std::string& name, int32_t value) const;
    void setUniformFloat(const std::string& name, float value) const;
    void setUniformVec2(const std::string& name, float x, float y) const;
    void setUniformVec3(const std::string& name, float x, float y, float z) const;
    void setUniformVec4(const std::string& name, float x, float y, float z, float w) const;
    void setUniformMat4(const std::string& name, const float* matrix, bool transpose = false) const;

    // 设置 Uniform（GLM 便捷方法）
    void setUniformVec2(const std::string& name, const glm::vec2& v) const;
    void setUniformVec3(const std::string& name, const glm::vec3& v) const;
    void setUniformVec4(const std::string& name, const glm::vec4& v) const;
    void setUniformMat4(const std::string& name, const glm::mat4& matrix, bool transpose = false) const;

    // 状态查询
    bool isValid() const { return mProgram != INVALID_HANDLE && mLinked; }
    bool isLinked() const { return mLinked; }
    const std::string& getName() const { return mName; }

private:
    // 反射获取属性和 Uniform
    void reflectAttributes();
    void reflectUniforms();
};

#endif // SHADER_H
