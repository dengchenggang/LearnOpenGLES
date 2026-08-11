#ifndef VERTEX_ARRAY_HPP
#define VERTEX_ARRAY_HPP

#include "IRenderInterface.h"
#include "Attribute.hpp"
#include <vector>

namespace engine {
namespace renderer {


// 分批属性描述
struct BatchDesc {
    Attribute attribute;    // 属性描述
    uint32_t  count;        // 元素数量
};

class VertexArray {
public:
    VertexArray() = default;
    ~VertexArray() { release(); }

    VertexArray(const VertexArray&) = delete;
    VertexArray& operator=(const VertexArray&) = delete;
    VertexArray(VertexArray&&) = delete;
    VertexArray& operator=(VertexArray&&) = delete;

    // 初始化：创建空VBO，计算各属性偏移
    void setup(const std::vector<BatchDesc>& batches);

    // 更新特定属性的数据
    void updateAttribute(uint32_t location, const void* data, size_t size);

    // 设置索引数据
    void setIndexData(const void* data, size_t size, DataType type, uint32_t count);
    void updateIndexData(const void* data, size_t size);

    // 绑定/解绑/绘制
    void bind() const;
    void unbind() const;
    void drawArrays(DrawMode mode, uint32_t first, uint32_t count) const;
    void drawElements(DrawMode mode) const;

    void release();
    bool isValid() const { return mVao != INVALID_HANDLE; }

private:
    struct BatchInfo {
        uint32_t location;
        uint32_t elemSize;      // 单个元素大小
        uint32_t count;         // 元素数量
        size_t   offset;        // 在VBO中的偏移
        size_t   capacity;      // 容量（字节）
    };

    RenderResourceHandle mVao = INVALID_HANDLE;
    RenderResourceHandle mVbo = INVALID_HANDLE;
    RenderResourceHandle mIbo = INVALID_HANDLE;

    std::vector<BatchInfo> mBatches;
    size_t mVboCapacity = 0;

    DataType mIndexType = DataType::UShort;
    uint32_t mIndexCount = 0;
    size_t mIboCapacity = 0;

    uint32_t getDataTypeSize(DataType type) const;
};


} // namespace renderer
} // namespace engine

#endif