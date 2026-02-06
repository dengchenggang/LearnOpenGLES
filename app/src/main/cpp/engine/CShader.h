//
// Created by banma-3361 on 2025/11/19.
//

#ifndef C_SHADER_H
#define C_SHADER_H
#include <string>
#include <string_view>
#include <cstdint>
#include <utility>
#include <vector>
#include <map>

namespace engine {

class CShader {
private:
    using shader_type = std::pair<std::uint32_t, std::uint32_t>;
    using shader_container_type = std::vector<shader_type>;
private:
    const std::string mName;
    std::uint32_t mProgram;
    std::map<std::string, std::uint32_t> mAttributes;
    std::map<std::string, std::uint32_t> mUniforms;
public:
    explicit CShader(std::string&& name);
    explicit CShader(const std::string& name);
    ~CShader();
    CShader(const CShader&) = delete;
    CShader& operator =(const CShader&) = delete;
public:
    explicit operator bool() const noexcept { return mProgram > 0; }
    void bind() const;
    void unbind() const;

private:
    bool compile(std::uint32_t shaderType, const char* code, shader_container_type& shaders) const;
    bool link(shader_container_type& shaders);
    void populateAttributes();
    void populateUniforms();
};

}  // namespace engine


#endif //LEARNOPENGLES_CSHADER_H
