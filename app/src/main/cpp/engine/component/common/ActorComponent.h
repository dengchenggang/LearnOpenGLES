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
    virtual void onBeginPlay() = 0;
    virtual void onEnabledChanged(bool enabled) = 0;
    virtual void onUpdate(float deltaTime) = 0;
    virtual void onEndPlay() = 0;
private:
    Actor& mOwner;
};

using ActorComponentPtr = std::unique_ptr<ActorComponent>;

} // namespace engine
