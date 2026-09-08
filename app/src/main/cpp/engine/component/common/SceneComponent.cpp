#include "SceneComponent.h"

namespace engine {

SceneComponent::SceneComponent(Actor& owner)
    : ActorComponent(owner)
    , mTransform(std::make_unique<Transform>()) {}

SceneComponent::SceneComponent(Actor& owner, TransformPtr&& transform)
    : ActorComponent(owner)
    , mTransform(std::move(transform)) {}

} // namespace engine
