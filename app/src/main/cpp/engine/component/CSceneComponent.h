#pragma once
#include "CActorComponent.h"
#include "Transform.h"

namespace engine {

class CSceneComponent : public CActorComponent {
public:
    explicit CSceneComponent(Actor& owner);
    explicit CSceneComponent(Actor& owner, TransformPtr&& transform);


    ~CSceneComponent() override = default;
    CSceneComponent(const CSceneComponent&) = delete;
    CSceneComponent& operator=(const CSceneComponent&) = delete;

    Transform& getTransform() const { return *mTransform; }

private:
    TransformPtr mTransform;
};

} // namespace engine
