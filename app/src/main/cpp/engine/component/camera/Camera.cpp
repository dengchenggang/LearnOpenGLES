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

const glm::mat4& Camera::getViewMatrix() const {
    if (mViewDirty) {
        mViewMatrix = glm::lookAt(mPosition, mTarget, mUp);
        mViewDirty = false;
    }
    return mViewMatrix;
}

const glm::mat4& Camera::getProjectionMatrix() const {
    if (mProjectionDirty) {
        float aspect = mViewportWidth / mViewportHeight;
        mProjectionMatrix = glm::perspective(glm::radians(mFov), aspect, mNearPlane, mFarPlane);
        mProjectionDirty = false;
    }
    return mProjectionMatrix;
}

const glm::mat4& Camera::getOrthoMatrix() const {
    if (mOrthoDirty) {
        mOrthoMatrix = glm::ortho(mViewportLeft, mViewportLeft + mViewportWidth,
                                  mViewportBottom, mViewportBottom + mViewportHeight,
                                  0.0f, mFarPlane);
        mOrthoDirty = false;
    }
    return mOrthoMatrix;
}

} // namespace engine
