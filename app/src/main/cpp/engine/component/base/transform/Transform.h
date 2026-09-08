#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <utility>
#include <memory>

namespace engine {

class Transform {
public:
    Transform();
    virtual ~Transform();

    Transform(const Transform&) = delete;
    Transform(Transform&&) = delete;
    Transform& operator=(const Transform&) = delete;
    Transform& operator=(Transform&&) = delete;
public:
    Transform& setPosition(const glm::vec3& position);
    Transform& setPosition(float x, float y, float z);
    Transform& setPositionX(float x);
    Transform& setPositionY(float y);
    Transform& setPositionZ(float z);

    const glm::vec3& getPosition() const { return mPosition; }

    Transform& rotate(const glm::vec3& eulerAngles);
    Transform& rotate(float x, float y, float z);
    Transform& move(const glm::vec3& vec);
    Transform& move(float x, float y, float z);

    Transform& setScale(float scale);
    Transform& setScale(const glm::vec3& scale);
    Transform& setScale(float x, float y, float z);
    Transform& setScaleX(float x);
    Transform& setScaleY(float y);
    Transform& setScaleZ(float z);

    const glm::vec3& getScale() const { return mScale; }

    void getEuler(float& x, float& y, float& z) const;

    static glm::quat cvtEulerAnglesToQuat(const glm::vec3& eulerAngles);
    static glm::quat cvtEulerAnglesToQuat(float pitch, float yaw, float roll);
    static void cvtQuatToEulerAngles(const glm::quat& q, float& pitch, float& yaw, float& roll);

    virtual const glm::mat4& getModelMatrix() const;

protected:
    void markDirty() { mModelDirty = true; }

    mutable bool mModelDirty = true;
    mutable glm::mat4 mModelMatrix = glm::mat4(1.0f);

    glm::vec3 mPosition;
    glm::quat mRotation;
    glm::vec3 mScale;
};

using TransformPtr = std::unique_ptr<Transform>;
} // namespace engine
