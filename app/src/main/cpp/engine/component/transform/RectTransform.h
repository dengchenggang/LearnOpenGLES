#pragma once
#include "Transform.h"

namespace engine {

class RectTransform : public Transform {
public:
    RectTransform();

    RectTransform& setPivot(const glm::vec2& pivot);
    const glm::vec2& getPivot() const { return mPivot; }

    RectTransform& setSize(const glm::vec2& size);
    const glm::vec2& getSize() const { return mSize; }

    const glm::mat4& getModelMatrix() const override;

private:
    glm::vec2 mPivot = glm::vec2(0.5f, 0.5f);
    glm::vec2 mSize = glm::vec2(100.0f, 100.0f);
};

using RectTransformPtr = std::unique_ptr<RectTransform>;

} // namespace engine
