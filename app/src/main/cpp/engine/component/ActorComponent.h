#pragma once
#include <memory>

namespace engine {

class Actor;

class ActorComponent {
public:
    explicit ActorComponent(Actor& owner);
    virtual ~ActorComponent() = default;
    ActorComponent(const ActorComponent&) = delete;
    ActorComponent& operator=(const ActorComponent&) = delete;

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

using ActorComponentPtr = std::unique_ptr<ActorComponent>;

} // namespace engine
