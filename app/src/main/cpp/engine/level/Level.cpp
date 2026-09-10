#include "Level.h"
#include "ILevelManager.h"

namespace engine {

Level::Level(const std::string& name, ILevelManager& levelManager)
    : mLevelManager(levelManager)
    , mName(name)
{
}

Level::~Level() {
}

void Level::onBeginPlay() {
    for (auto& actor : mActors) {
        actor->onBeginPlay();
    }
}

void Level::onUpdate(int64_t deltaTime) {
    for (auto& actor : mActors) {
        if (actor->isEnabled()) {
            actor->onUpdate(deltaTime);
        }
    }
}

void Level::onRender() {
    for (auto& actor : mActors) {
        if (actor->isVisible()) {
            actor->onRender();
        }
    }
}

void Level::onEndPlay() {
    for (auto& actor : mActors) {
        actor->onEndPlay();
    }
}

Actor& Level::createActor() {
    mActors.emplace_back(new Actor());
    return *mActors.back();
}

void Level::changeLevel(const std::string& levelName) {
    mLevelManager.changeLevel(levelName);
}

} // namespace engine