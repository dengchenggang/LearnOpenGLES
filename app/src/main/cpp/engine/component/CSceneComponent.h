#pragma once
#include "CActorComponent.h"

namespace engine {


class CSceneComponent : public CActorComponent {
public:
    CSceneComponent() = default;
    ~CSceneComponent() override = default;
    CSceneComponent(const CSceneComponent&) = delete;
    CSceneComponent& operator=(const CSceneComponent&) = delete;
public:

};

} // namespace engine
