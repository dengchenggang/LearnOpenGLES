#include "Camera.h"

namespace engine {

Camera::Camera() = default;

void Camera::setPosition(const glm::vec3& position) {
    mPosition = position;
    markViewDirty();
}

void Camera::setTarget(const glm::vec3& target) {
    mTarget = target;
    markViewDirty();
}

void Camera::setUp(const glm::vec3& up) {
    mUp = up;
    markViewDirty();
}

void Camera::setViewport(float left, float bottom, float width, float height) {
    mViewportLeft = left;
    mViewportBottom = bottom;
    mViewportWidth = width;
    mViewportHeight = height;
    markProjectionDirty();
}

void Camera::setPerspective(float fov, float nearPlane, float farPlane) {
    mFov = fov;
    mNearPlane = nearPlane;
    mFarPlane = farPlane;
    markProjectionDirty();
}

const std::pair<glm::mat4, uint64_t>& Camera::getViewMatrix() const {
    if (mViewDirty) {
        mViewMatrix.first = glm::lookAt(mPosition, mTarget, mUp);
        ++mViewMatrix.second;
        mViewDirty = false;
    }
    return mViewMatrix;
}

const std::pair<glm::mat4, uint64_t>& Camera::getProjectionMatrix() const {
    if (mProjectionDirty) {
        float aspect = mViewportWidth / mViewportHeight;
        mProjectionMatrix.first = glm::perspective(glm::radians(mFov), aspect, mNearPlane, mFarPlane);
        ++mProjectionMatrix.second;
        mProjectionDirty = false;
    }
    return mProjectionMatrix;
}

const std::pair<glm::mat4, uint64_t>& Camera::getOrthoMatrix() const {
    if (mOrthoDirty) {
        mOrthoMatrix.first = glm::ortho(mViewportLeft, mViewportLeft + mViewportWidth,
                                        mViewportBottom, mViewportBottom + mViewportHeight,
                                        0.0f, mFarPlane);
        ++mOrthoMatrix.second;
        mOrthoDirty = false;
    }
    return mOrthoMatrix;
}

} // namespace engine
