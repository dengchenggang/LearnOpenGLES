#pragma once
#include "ActorComponent.h"
#include "Transform.h"
#include <vector>
#include <glm/glm.hpp>

namespace engine {

class SceneComponent : public ActorComponent {
    friend class Actor;
public:
    explicit SceneComponent(Actor& owner);
    explicit SceneComponent(Actor& owner, TransformPtr&& transform);
    ~SceneComponent() override;
    SceneComponent(const SceneComponent&) = delete;
    SceneComponent& operator=(const SceneComponent&) = delete;
public:
    Transform& getTransform() const { return *mTransform; }
    const glm::mat4& getLocalMatrix() const { return mTransform->getModelMatrix(); }
    glm::mat4 getWorldMatrix() const;
    glm::vec3 getWorldPosition() const;

    /**
     * 将当前组件挂载到指定的父组件下，形成场景图层级关系。
     * 挂载后父组件的变换矩阵会级联影响当前组件，生命周期遍历也会按树形顺序传播。
     * @param parent 目标父组件；传入 nullptr 等效于调用 detachFromParent()
     *
     * 调用时机：通常由 Actor::attachToActor() 触发，也可在组件逻辑中直接调用。
     * 内部会自动处理：解除原挂载关系、防循环挂载检测、触发 onAttachedTo 回调。
     */
    void attachTo(SceneComponent* parent);
    void detachFromParent();
    SceneComponent* getAttachParent() const { return mAttachParent; }
    const std::vector<SceneComponent*>& getAttachChildren() const { return mAttachChildren; }

protected:
    /**
     * 当前组件成功挂载到父组件后的回调。
     * @param parent 挂载目标父组件，不会为 nullptr
     *
     * 调用时机：attachTo() 内部建立父子关系之后立即调用。
     * 派生类可重写此方法，在挂载时同步父级状态或执行自定义初始化逻辑。
     */
    virtual void onAttachedTo(SceneComponent* parent) {}
    /**
     * 当前组件从父组件卸载后的回调。
     * @param parent 原父组件，不会为 nullptr
     *
     * 调用时机：detachFromParent() 内部解除父子关系之后立即调用；
     * 组件析构时也会先触发 detachFromParent()，进而调用此方法。
     * 派生类可重写此方法，在卸载时清理与父级相关的状态。
     */
    virtual void onDetachedFrom(SceneComponent* parent) {}

private:
    void markWorldDirty();

private:
    TransformPtr mTransform;
    SceneComponent* mAttachParent = nullptr;
    std::vector<SceneComponent*> mAttachChildren;
};

} // namespace engine
