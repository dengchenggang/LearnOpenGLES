#include "RectTransform.h"
#include <glm/gtc/matrix_transform.hpp>

namespace engine {

RectTransform::RectTransform() = default;

RectTransform& RectTransform::setPivot(const glm::vec2& pivot) {
    mPivot = pivot;
    markDirty();
    return *this;
}

RectTransform& RectTransform::setSize(const glm::vec2& size) {
    mSize = size;
    markDirty();
    return *this;
}

const glm::mat4& RectTransform::getModelMatrix() const {
    if (mModelDirty) {
        glm::vec3 pivotOffset(-mPivot.x * mSize.x, -mPivot.y * mSize.y, 0.0f);

        mModelMatrix = glm::translate(glm::mat4(1.0f), mPosition)
                     * glm::mat4_cast(mRotation)
                     * glm::scale(glm::mat4(1.0f), mScale)
                     * glm::translate(glm::mat4(1.0f), pivotOffset);
        mModelDirty = false;
    }
    return mModelMatrix;
}

} // namespace engine
