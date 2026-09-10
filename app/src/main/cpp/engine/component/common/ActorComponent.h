#pragma once
#include <memory>

namespace engine {

class Actor;

class ActorComponent {
    friend class Actor;
public:
    explicit ActorComponent(Actor& owner);
    virtual ~ActorComponent();
    ActorComponent(const ActorComponent&) = delete;
    ActorComponent& operator=(const ActorComponent&) = delete;
public:
    Actor& GetOwner() const { return mOwner; }
protected:
    virtual void onAttach() {}
    virtual void onBeginPlay() {}
    virtual void onEnabledChanged(bool enabled) {}
    virtual void onVisibilityChanged(bool visible) {}
    virtual void onUpdate(float deltaTime) {}
    virtual void onRender() {}
    virtual void onEndPlay() {}
private:
    Actor& mOwner;
};

using ActorComponentPtr = std::unique_ptr<ActorComponent>;

} // namespace engine
