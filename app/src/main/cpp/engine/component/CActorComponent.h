#pragma once
#include <memory>

namespace engine {


class CActor;

class CActorComponent {
public:
    CActorComponent(CActor& owner);
    virtual ~CActorComponent() = default;
    CActorComponent(const CActorComponent&) = delete;
    CActorComponent& operator=(const CActorComponent&) = delete;
public:
    CActor& GetOwner() const { return mOwner; }
private:
    CActor& mOwner;
};

using CActorComponentPtr = std::unique_ptr<CActorComponent>;

} // namespace engine
