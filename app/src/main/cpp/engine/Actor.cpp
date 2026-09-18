#include "Actor.h"
#include "SceneComponent.h"

namespace engine {

Actor::Actor(Level& level)
    : mLevel(level)
    , mEnabled(false, true)
    , mVisible(false, true)
{

}

Actor::~Actor() {}

Actor& Actor::setEnabled(bool enabled) {
    if (mEnabled.first != enabled) {
        mEnabled.first = enabled;
        mEnabled.second = true;
    }
    return *this;
}

Actor& Actor::setVisible(bool visible) {
    if (mVisible.first != visible) {
        mVisible.first = visible;
        mVisible.second = true;
    }
    return *this;
}

void Actor::attachToActor(Actor& parent) {
    if (!mRootComponent) {
        return;
    }
    auto* parentRoot = parent.getRootComponent();
    if (!parentRoot) {
        return;
    }
    mRootComponent->attachTo(parentRoot);
}

void Actor::detachFromActor() {
    if (!mRootComponent) {
        return;
    }
    mRootComponent->detachFromParent();
}

Actor* Actor::getAttachParentActor() const {
    if (!mRootComponent) {
        return nullptr;
    }
    auto* parentComp = mRootComponent->getAttachParent();
    if (!parentComp) {
        return nullptr;
    }
    return &parentComp->GetOwner();
}

void Actor::onBeginPlay() {
    for (auto& comp : mComponents) {
        comp->onBeginPlay();
    }
    if (mRootComponent) {
        for (auto* childComp : mRootComponent->getAttachChildren()) {
            Actor* childActor = &childComp->GetOwner();
            if (childActor != this) {
                childActor->onBeginPlay();
            }
        }
    }
}

void Actor::onUpdate(float deltaTime) {
    if (mEnabled.second) {
        for (auto& comp : mComponents) {
            comp->onEnabledChanged(mEnabled.first);
        }
        mEnabled.second = false;
    }

    if (mVisible.second) {
        for (auto& comp : mComponents) {
            comp->onVisibilityChanged(mVisible.first);
        }
        mVisible.second = false;
    }

    if (mEnabled.first) {
        for (auto& comp : mComponents) {
            comp->onUpdate(deltaTime);
        }
        if (mRootComponent) {
            for (auto* childComp : mRootComponent->getAttachChildren()) {
                Actor* childActor = &childComp->GetOwner();
                if (childActor != this) {
                    childActor->onUpdate(deltaTime);
                }
            }
        }
    }
}

void Actor::onRender() {
    if (!mVisible.first) {
        return;
    }
    for (auto& comp : mComponents) {
        comp->onRender();
    }
    if (mRootComponent) {
        for (auto* childComp : mRootComponent->getAttachChildren()) {
            Actor* childActor = &childComp->GetOwner();
            if (childActor != this) {
                childActor->onRender();
            }
        }
    }
}

void Actor::onEndPlay() {
    for (auto& comp : mComponents) {
        comp->onEndPlay();
    }
    if (mRootComponent) {
        for (auto* childComp : mRootComponent->getAttachChildren()) {
            Actor* childActor = &childComp->GetOwner();
            if (childActor != this) {
                childActor->onEndPlay();
            }
        }
    }
}

} // namespace engine
