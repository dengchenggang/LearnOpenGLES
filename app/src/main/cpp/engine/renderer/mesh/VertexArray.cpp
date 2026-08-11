#include "VertexArray.h"
#include "RenderInterface.h"

namespace engine {
namespace renderer {


void VertexArray::setup(const std::vector<BatchDesc>& batches) {
    // 计算总容量和各属性偏移
    size_t totalOffset = 0;
    for (const auto& batch : batches) {
        uint32_t elemSize = getDataTypeSize(batch.attribute.type) * batch.attribute.size;
        size_t capacity = elemSize * batch.count;

        mBatches.push_back({
            batch.attribute.location,
            elemSize,
            batch.count,
            totalOffset,
            capacity
        });

        totalOffset += capacity;
    }
    mVboCapacity = totalOffset;

    // 创建VAO和空VBO
    mVao = RenderInterface.createVertexArray();
    mVbo = RenderInterface.createVertexBuffer(nullptr, mVboCapacity, BufferUsage::Dynamic);

    RenderInterface.bindVertexArray(mVao);
    RenderInterface.bindVertexBuffer(mVbo);

    // 配置属性（分批布局，stride=0表示紧密排列）
    for (size_t i = 0; i < mBatches.size(); ++i) {
        const auto& info = mBatches[i];
        const auto& desc = batches[i];
        RenderInterface.enableVertexAttrib(info.location);
        RenderInterface.setVertexAttribPointer(
            info.location,
            desc.attribute.size,
            desc.attribute.type,
            desc.attribute.normalized,
            0,  // stride=0，每个属性单独一块
            reinterpret_cast<void*>(info.offset)
        );
    }

    RenderInterface.bindVertexArray(INVALID_HANDLE);
}

void VertexArray::updateAttribute(uint32_t location, const void* data, size_t size) {
    for (const auto& info : mBatches) {
        if (info.location == location) {
            size_t updateSize = (size > info.capacity) ? info.capacity : size;
            RenderInterface.updateBufferData(mVbo, info.offset, updateSize, data);
            break;
        }
    }
}

void VertexArray::setIndexData(const void* data, size_t size, DataType type, uint32_t count) {
    mIboCapacity = size;
    mIndexType = type;
    mIndexCount = count;
    mIbo = RenderInterface.createIndexBuffer(data, size, BufferUsage::Dynamic);
}

void VertexArray::updateIndexData(const void* data, size_t size) {
    if (mIbo != INVALID_HANDLE) {
        size_t updateSize = (size > mIboCapacity) ? mIboCapacity : size;
        RenderInterface.updateBufferData(mIbo, 0, updateSize, data);
    }
}

void VertexArray::bind() const {
    RenderInterface.bindVertexArray(mVao);
}

void VertexArray::unbind() const {
    RenderInterface.bindVertexArray(INVALID_HANDLE);
}

void VertexArray::drawArrays(DrawMode mode, uint32_t first, uint32_t count) const {
    RenderInterface.drawArrays(mode, first, count);
}

void VertexArray::drawElements(DrawMode mode) const {
    if (mIbo != INVALID_HANDLE) {
        RenderInterface.bindIndexBuffer(mIbo);
        RenderInterface.drawElements(mode, mIndexCount, mIndexType, nullptr);
    }
}

void VertexArray::release() {
    if (mVao != INVALID_HANDLE) {
        RenderInterface.deleteVertexArray(mVao);
        mVao = INVALID_HANDLE;
    }
    if (mVbo != INVALID_HANDLE) {
        RenderInterface.deleteBuffer(mVbo);
        mVbo = INVALID_HANDLE;
    }
    if (mIbo != INVALID_HANDLE) {
        RenderInterface.deleteBuffer(mIbo);
        mIbo = INVALID_HANDLE;
    }
    mBatches.clear();
    mVboCapacity = 0;
    mIndexCount = 0;
}

uint32_t VertexArray::getDataTypeSize(DataType type) const {
    switch (type) {
        case DataType::Byte:
        case DataType::UByte: return 1;
        case DataType::Short:
        case DataType::UShort: return 2;
        case DataType::Int:
        case DataType::UInt:
        case DataType::Float: return 4;
    }
    return 4;
}

} // namespace renderer
} // namespace engine
