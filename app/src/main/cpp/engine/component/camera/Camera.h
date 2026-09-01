#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace engine {

class Camera {
public:
    Camera();
    ~Camera() = default;

    Camera(const Camera&) = delete;
    Camera& operator=(const Camera&) = delete;

    void setPosition(const glm::vec3& position);
    void setTarget(const glm::vec3& target);
    void setUp(const glm::vec3& up);

    void setPerspective(float fov, float aspect, float nearPlane, float farPlane);
    void setOrtho(float left, float right, float bottom, float top, float nearPlane, float farPlane);

    const glm::mat4& getViewMatrix() const;
    const glm::mat4& getProjectionMatrix() const;

    const glm::vec3& getPosition() const { return mPosition; }
    const glm::vec3& getTarget() const { return mTarget; }

private:
    void markViewDirty() { mViewDirty = true; }
    void markProjectionDirty() { mProjectionDirty = true; }

    mutable bool mViewDirty = true;
    mutable bool mProjectionDirty = true;
    mutable glm::mat4 mViewMatrix = glm::mat4(1.0f);
    mutable glm::mat4 mProjectionMatrix = glm::mat4(1.0f);

    glm::vec3 mPosition = glm::vec3(0.0f, 0.0f, 5.0f);
    glm::vec3 mTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 mUp = glm::vec3(0.0f, 1.0f, 0.0f);

    float mFov = 45.0f;
    float mAspect = 16.0f / 9.0f;
    float mNearPlane = 0.1f;
    float mFarPlane = 100.0f;

    float mLeft = -1.0f, mRight = 1.0f;
    float mBottom = -1.0f, mTop = 1.0f;
    bool mIsPerspective = true;
};

} // namespace engine
