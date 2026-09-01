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

void Camera::setPerspective(float fov, float aspect, float nearPlane, float farPlane) {
    mFov = fov;
    mAspect = aspect;
    mNearPlane = nearPlane;
    mFarPlane = farPlane;
    mIsPerspective = true;
    markProjectionDirty();
}

void Camera::setOrtho(float left, float right, float bottom, float top, float nearPlane, float farPlane) {
    mLeft = left;
    mRight = right;
    mBottom = bottom;
    mTop = top;
    mNearPlane = nearPlane;
    mFarPlane = farPlane;
    mIsPerspective = false;
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
        if (mIsPerspective) {
            mProjectionMatrix = glm::perspective(glm::radians(mFov), mAspect, mNearPlane, mFarPlane);
        } else {
            mProjectionMatrix = glm::ortho(mLeft, mRight, mBottom, mTop, mNearPlane, mFarPlane);
        }
        mProjectionDirty = false;
    }
    return mProjectionMatrix;
}

} // namespace engine
