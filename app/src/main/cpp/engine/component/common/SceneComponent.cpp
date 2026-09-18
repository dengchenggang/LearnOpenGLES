#include "SceneComponent.h"
#include <algorithm>

namespace engine {

SceneComponent::SceneComponent(Actor& owner)
    : ActorComponent(owner)
    , mTransform(std::make_unique<Transform>()) {}

SceneComponent::SceneComponent(Actor& owner, TransformPtr&& transform)
    : ActorComponent(owner)
    , mTransform(std::move(transform)) {}

SceneComponent::~SceneComponent() {
    detachFromParent();
    auto children = mAttachChildren;
    for (auto* child : children) {
        child->detachFromParent();
    }
}

void SceneComponent::attachTo(SceneComponent* parent) {
    if (mAttachParent == parent || parent == this) {
        return;
    }
    SceneComponent* check = parent;
    while (check) {
        if (check == this) {
            return;
        }
        check = check->mAttachParent;
    }
    detachFromParent();
    if (parent) {
        mAttachParent = parent;
        parent->mAttachChildren.push_back(this);
        onAttachedTo(parent);
    }
}

void SceneComponent::detachFromParent() {
    if (!mAttachParent) {
        return;
    }
    auto* oldParent = mAttachParent;
    auto& siblings = oldParent->mAttachChildren;
    siblings.erase(std::remove(siblings.begin(), siblings.end(), this), siblings.end());
    mAttachParent = nullptr;
    onDetachedFrom(oldParent);
}

glm::mat4 SceneComponent::getWorldMatrix() const {
    if (mAttachParent) {
        return mAttachParent->getWorldMatrix() * getLocalMatrix();
    }
    return getLocalMatrix();
}

glm::vec3 SceneComponent::getWorldPosition() const {
    return glm::vec3(getWorldMatrix()[3]);
}

void SceneComponent::markWorldDirty() {
    for (auto* child : mAttachChildren) {
        child->markWorldDirty();
    }
}

} // namespace engine
