#pragma once
#include "ActorComponent.h"
#include "Transform.h"

namespace engine {

class SceneComponent : public ActorComponent {
public:
    explicit SceneComponent(Actor& owner);
    explicit SceneComponent(Actor& owner, TransformPtr&& transform);


    ~SceneComponent() override = default;
    SceneComponent(const SceneComponent&) = delete;
    SceneComponent& operator=(const SceneComponent&) = delete;

    Transform& getTransform() const { return *mTransform; }

private:
    TransformPtr mTransform;
};

} // namespace engine
