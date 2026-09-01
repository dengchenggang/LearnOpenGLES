#pragma once
#include <memory>

namespace engine {

class Actor;

class CActorComponent {
public:
    explicit CActorComponent(Actor& owner);
    virtual ~CActorComponent() = default;
    CActorComponent(const CActorComponent&) = delete;
    CActorComponent& operator=(const CActorComponent&) = delete;

    virtual void onAttach() {}
    virtual void onBeginPlay() {}
    virtual void onUpdate(float deltaTime) {}
    virtual void onRender() {}
    virtual void onEndPlay() {}

    Actor& GetOwner() const { return mOwner; }

    bool isEnabled() const { return mEnabled; }
    void setEnabled(bool enabled) { mEnabled = enabled; }

private:
    Actor& mOwner;
    bool mEnabled = true;
};

using CActorComponentPtr = std::unique_ptr<CActorComponent>;

} // namespace engine
