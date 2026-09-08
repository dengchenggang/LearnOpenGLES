#include "Transform.h"
#include <glm/gtc/matrix_transform.hpp>

namespace engine {

Transform::Transform()
    : mPosition(0.0f, 0.0f, 0.0f)
    , mRotation(1.0f, 0.0f, 0.0f, 0.0f)
    , mScale(1.0f, 1.0f, 1.0f) {}

Transform::~Transform() = default;

Transform& Transform::setPosition(const glm::vec3& position) {
    mPosition = position;
    markDirty();
    return *this;
}

Transform& Transform::setPosition(float x, float y, float z) {
    mPosition = glm::vec3(x, y, z);
    markDirty();
    return *this;
}

Transform& Transform::setPositionX(float x) {
    mPosition.x = x;
    markDirty();
    return *this;
}

Transform& Transform::setPositionY(float y) {
    mPosition.y = y;
    markDirty();
    return *this;
}

Transform& Transform::setPositionZ(float z) {
    mPosition.z = z;
    markDirty();
    return *this;
}

Transform& Transform::rotate(const glm::vec3& eulerAngles) {
    mRotation *= cvtEulerAnglesToQuat(eulerAngles);
    markDirty();
    return *this;
}

Transform& Transform::rotate(float x, float y, float z) {
    mRotation *= cvtEulerAnglesToQuat(x, y, z);
    markDirty();
    return *this;
}

Transform& Transform::move(const glm::vec3& vec) {
    mPosition += vec;
    markDirty();
    return *this;
}

Transform& Transform::move(float x, float y, float z) {
    mPosition += glm::vec3(x, y, z);
    markDirty();
    return *this;
}

Transform& Transform::setScale(float scale) {
    mScale = glm::vec3(scale, scale, scale);
    markDirty();
    return *this;
}

Transform& Transform::setScale(const glm::vec3& scale) {
    mScale = scale;
    markDirty();
    return *this;
}

Transform& Transform::setScale(float x, float y, float z) {
    mScale = glm::vec3(x, y, z);
    markDirty();
    return *this;
}

Transform& Transform::setScaleX(float x) {
    mScale.x = x;
    markDirty();
    return *this;
}

Transform& Transform::setScaleY(float y) {
    mScale.y = y;
    markDirty();
    return *this;
}

Transform& Transform::setScaleZ(float z) {
    mScale.z = z;
    markDirty();
    return *this;
}

void Transform::getEuler(float& x, float& y, float& z) const {
    cvtQuatToEulerAngles(mRotation, x, y, z);
}

glm::quat Transform::cvtEulerAnglesToQuat(const glm::vec3& eulerAngles) {
    return cvtEulerAnglesToQuat(eulerAngles.x, eulerAngles.y, eulerAngles.z);
}

glm::quat Transform::cvtEulerAnglesToQuat(float pitch, float yaw, float roll) {
    return glm::quat(glm::vec3(pitch, yaw, roll));
}

void Transform::cvtQuatToEulerAngles(const glm::quat& q, float& pitch, float& yaw, float& roll) {
    glm::vec3 euler = glm::eulerAngles(q);
    pitch = euler.x;
    yaw   = euler.y;
    roll  = euler.z;
}

const glm::mat4& Transform::getModelMatrix() const {
    if (mModelDirty) {
        mModelMatrix = glm::translate(glm::mat4(1.0f), mPosition)
                     * glm::mat4_cast(mRotation)
                     * glm::scale(glm::mat4(1.0f), mScale);
        mModelDirty = false;
    }
    return mModelMatrix;
}
} // namespace engine
