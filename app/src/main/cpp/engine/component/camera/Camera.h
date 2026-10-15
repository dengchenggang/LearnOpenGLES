#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

namespace engine {

class Camera {
public:
    // 构造函数
    Camera();
    // 析构函数
    ~Camera() = default;

    Camera(const Camera&) = delete;
    Camera& operator=(const Camera&) = delete;
public:
    /**
     * 设置相机在世界空间中的位置
     * @param position 相机位置坐标
     */
    void setPosition(const glm::vec3& position);

    /**
     * 设置相机观察的目标点
     * @param target 目标点坐标
     */
    void setTarget(const glm::vec3& target);

    /**
     * 设置相机的上方向向量
     * @param up 上方向向量，通常为单位向量 (0, 1, 0)
     */
    void setUp(const glm::vec3& up);

    /**
     * 设置逻辑视口矩形，由 Level 的设计尺寸适配后得出，并非物理像素
     * 正交投影范围为 [left, left + width] x [bottom, bottom + height]
     * @param left   逻辑左边界
     * @param bottom 逻辑下边界
     * @param width  逻辑宽度
     * @param height 逻辑高度
     */
    void setViewport(float left, float bottom, float width, float height);

    /**
     * 设置透视投影参数，fov 为角度制
     * @param fov        垂直视野角度（度）
     * @param nearPlane  近裁剪面距离
     * @param farPlane   远裁剪面距离
     */
    void setPerspective(float fov, float nearPlane, float farPlane);

    /**
     * 获取观察矩阵（懒计算，内部缓存）
     * @return 当前相机的观察矩阵
     */
    const glm::mat4& getViewMatrix() const;

    /**
     * 获取投影矩阵（懒计算，内部缓存）
     * @return 当前相机的透视投影矩阵
     */
    const glm::mat4& getProjectionMatrix() const;

    /**
     * 获取正交投影矩阵（懒计算，内部缓存），范围由 setViewport 指定的逻辑矩形决定
     * @return 当前相机的正交投影矩阵
     */
    const glm::mat4& getOrthoMatrix() const;

    /**
     * 获取相机当前位置
     * @return 相机位置坐标
     */
    const glm::vec3& getPosition() const { return mPosition; }

    /**
     * 获取当前观察目标点
     * @return 目标点坐标
     */
    const glm::vec3& getTarget() const { return mTarget; }

private:
    // 标记观察矩阵需要重新计算
    void markViewDirty() { mViewDirty = true; }
    // 标记投影矩阵需要重新计算
    void markProjectionDirty() {
        mProjectionDirty = true;
        mOrthoDirty = true;
    }
private:
    mutable bool mViewDirty = true;
    mutable bool mProjectionDirty = true;
    mutable bool mOrthoDirty = true;
    mutable glm::mat4 mViewMatrix = glm::mat4(1.0f);
    mutable glm::mat4 mProjectionMatrix = glm::mat4(1.0f);
    mutable glm::mat4 mOrthoMatrix = glm::mat4(1.0f);

    glm::vec3 mPosition = glm::vec3(0.0f, 0.0f, 5.0f);
    glm::vec3 mTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 mUp = glm::vec3(0.0f, 1.0f, 0.0f);

    float mViewportLeft = 0.0f;
    float mViewportBottom = 0.0f;
    float mViewportWidth = 16.0f;
    float mViewportHeight = 9.0f;
    float mFov = 45.0f;
    float mNearPlane = 0.1f;
    float mFarPlane = 100.0f;
};

using CameraPtr = std::unique_ptr<Camera>;
} // namespace engine
