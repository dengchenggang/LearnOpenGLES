#include "CSceneComponent.h"

namespace engine {

CSceneComponent::CSceneComponent(Actor& owner)
    : CActorComponent(owner)
    , mTransform(std::make_unique<Transform>()) {}

CSceneComponent::CSceneComponent(Actor& owner, TransformPtr&& transform)
    : CActorComponent(owner)
    , mTransform(std::move(transform)) {}

} // namespace engine
