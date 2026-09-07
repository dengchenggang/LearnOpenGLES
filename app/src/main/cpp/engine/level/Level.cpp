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

void Level::beginPlay() {
    for (auto& actor : mActors) {
        if (actor->isActive()) {
            actor->beginPlay();
        }
    }
}

void Level::update(int64_t deltaTime) {
    for (auto& actor : mActors) {
        if (actor->isActive()) {
            actor->update(deltaTime);
        }
    }
}

void Level::render() {
    for (auto& actor : mActors) {
        if (actor->isActive()) {
            actor->render();
        }
    }
}

void Level::endPlay() {
    for (auto& actor : mActors) {
        if (actor->isActive()) {
            actor->endPlay();
        }
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