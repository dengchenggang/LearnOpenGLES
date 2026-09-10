#include "Actor.h"

namespace engine {

Actor::Actor()
    : mEnabled(false, true)
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

void Actor::onBeginPlay() {
    for (auto& comp : mComponents) {
        comp->onBeginPlay();
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
    }
}

void Actor::onRender() {
    if (mVisible.first) {
        for (auto& comp : mComponents) {
            comp->onRender();
        }
    }
}

void Actor::onEndPlay() {
    for (auto& comp : mComponents) {
        comp->onEndPlay();
    }
}
} // namespace engine
