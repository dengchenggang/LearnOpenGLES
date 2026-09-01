#include "Actor.h"

namespace engine {

Actor::Actor() = default;

void Actor::beginPlay() {
    for (auto& comp : mComponents) {
        comp->onBeginPlay();
    }
}

void Actor::update(float deltaTime) {
    if (!mActive) return;
    for (auto& comp : mComponents) {
        if (comp->isEnabled()) {
            comp->onUpdate(deltaTime);
        }
    }
}

void Actor::render() {
    if (!mActive) return;
    for (auto& comp : mComponents) {
        if (comp->isEnabled()) {
            comp->onRender();
        }
    }
}

void Actor::endPlay() {
    for (auto& comp : mComponents) {
        comp->onEndPlay();
    }
}

} // namespace engine
