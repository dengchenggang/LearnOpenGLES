#pragma once
#include <vector>
#include "CActorComponent.h"

class CActor {
public:
    CActor() = default;
    virtual ~CActor() = default;
    CActor(const CActor&) = delete;
    CActor& operator=(const CActor&) = delete;
public:
private:
    std::vector<CActorComponentPtr> mComponents;
};
