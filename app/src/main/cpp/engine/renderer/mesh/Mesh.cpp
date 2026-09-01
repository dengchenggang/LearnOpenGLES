#include "Mesh.h"

namespace engine {
namespace renderer {

void Mesh::initialize(const std::vector<BatchDesc>& batches) {
    mVertexArray.setup(batches);
}

void Mesh::setAttributeData(uint32_t location, const void* data, size_t size) {
    mVertexArray.updateAttribute(location, data, size);
}

void Mesh::setIndexData(const void* data, size_t size, DataType type, uint32_t count) {
    mVertexArray.setIndexData(data, size, type, count);
    mHasIndexData = true;
}

void Mesh::updateIndexData(const void* data, size_t size) {
    mVertexArray.updateIndexData(data, size);
}

void Mesh::draw(DrawMode mode) const {
    mVertexArray.bind();
    if (mHasIndexData) {
        mVertexArray.drawElements(mode);
    } else if (mVertexCount > 0) {
        mVertexArray.drawArrays(mode, 0, mVertexCount);
    }
    mVertexArray.unbind();
}

} // namespace renderer
} // namespace engine
