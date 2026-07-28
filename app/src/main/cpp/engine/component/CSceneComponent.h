#pragma once
#include "CActorComponent.h"

class CSceneComponent : public CActorComponent {
public:
    CSceneComponent() = default;
    ~CSceneComponent() override = default;
    CSceneComponent(const CSceneComponent&) = delete;
    CSceneComponent& operator=(const CSceneComponent&) = delete;
public:

};
