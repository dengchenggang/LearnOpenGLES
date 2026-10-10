#include "Level.h"
#include "ILevelManager.h"
#include <algorithm>

namespace engine {

uint64_t Level::sNextActorId = 1;

Level::Level(const std::string& name, ILevelManager& levelManager)
    : mLevelManager(levelManager)
    , mName(name)
{
}

Level::~Level() {
}

void Level::onUpdate(int64_t deltaTime) {
    for (auto& actor : mActors) {
        if (!actor->getAttachParentActor() && actor->isEnabled()) {
            actor->onUpdate(deltaTime);
        }
    }
}

void Level::onRender() {
    for (auto& actor : mActors) {
        if (!actor->getAttachParentActor() && actor->isVisible()) {
            actor->onRender();
        }
    }
}

void Level::destroyActor(uint64_t actorId) {
    auto it = std::find_if(mActors.begin(), mActors.end(),
        [actorId](const ActorPtr& actor) { return actor->getId() == actorId; });
    if (it != mActors.end()) {
        (*it)->onEndPlay();
        (*it)->detachFromActor();
        mActors.erase(it);
    }
}

void Level::changeLevel(const std::string& levelName) {
    mLevelManager.changeLevel(levelName);
}

} // namespace engine