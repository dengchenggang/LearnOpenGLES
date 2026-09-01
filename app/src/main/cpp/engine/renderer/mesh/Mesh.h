#pragma once

#include "VertexArray.h"
#include <vector>

namespace engine {
namespace renderer {

class Mesh {
public:
    Mesh() = default;
    ~Mesh() = default;

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    Mesh(Mesh&&) = delete;
    Mesh& operator=(Mesh&&) = delete;

    // 初始化顶点属性布局
    void initialize(const std::vector<BatchDesc>& batches);

    // 上传指定 location 的顶点属性数据
    void setAttributeData(uint32_t location, const void* data, size_t size);

    // 设置索引数据（启用索引绘制）
    void setIndexData(const void* data, size_t size, DataType type, uint32_t count);
    void updateIndexData(const void* data, size_t size);

    // 设置顶点数量（用于无索引绘制）
    void setVertexCount(uint32_t count) { mVertexCount = count; }

    // 绘制
    void draw(DrawMode mode = DrawMode::Triangles) const;

    bool isValid() const { return mVertexArray.isValid(); }
    void bind() const { mVertexArray.bind(); }
    void unbind() const { mVertexArray.unbind(); }

private:
    VertexArray mVertexArray;
    uint32_t mVertexCount = 0;
    bool mHasIndexData = false;
};

} // namespace renderer
} // namespace engine
