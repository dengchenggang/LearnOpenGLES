#include "LevelManager.h"
#include "utils/Log.h"
#include "NilLevel.h"

namespace engine {

void LevelManager::changeLevel(const std::string& levelName) {
    LogI("%s --> %s", mActiveLevel ? mActiveLevel->getId().c_str() : "nullptr", levelName.c_str());
    mNextActiveLevelName = levelName;
}

void LevelManager::initLevels(std::map<std::string, LevelPtr>&& levels, Camera& camera) {
    mActiveLevel = nullptr;
    mNextActiveLevelName.clear();
    mLevels.clear();

    for (auto& pair : levels) {
        if (pair.second) {
            mLevels[pair.first] = std::move(pair.second);
        }
    }
    mLevels[NilLevel::NAME] = std::make_unique<NilLevel>(*this, camera);
}

void LevelManager::updateLevels(int64_t deltaTime) {
    if (!mNextActiveLevelName.empty()) {
        doChangeLevel(mNextActiveLevelName);
    }
    if (mActiveLevel) {
        mActiveLevel->onUpdate(deltaTime);
    }
}


void LevelManager::doChangeLevel(const std::string& levelName) {
    LogI("doChangeLevel: %s --> %s", mActiveLevel ? mActiveLevel->getId().c_str() : "nullptr", levelName.c_str());
    auto it = mLevels.find(levelName);
    if (it != mLevels.end()) {
        auto oldLevel = mActiveLevel;
        mActiveLevel = it->second.get();
        if (oldLevel) {
            mPreviousActiveLevelName = oldLevel->getId();
            oldLevel->onExit();
        }
        if (mActiveLevel) {
            mActiveLevel->onBeginPlay();
            mActiveLevel->onEnter(mViewportWidth, mViewportHeight);
        }
    } else {
        LogE("doChangeLevel: %s not found", levelName.c_str());
    }
    mNextActiveLevelName = "";
}

void LevelManager::renderLevels() {
    if (mActiveLevel) {
        mActiveLevel->onRender();
    }
}

void LevelManager::deInitLevels() {
    for (auto& pair : mLevels) {
        pair.second->onEndPlay();
    }
    mActiveLevel = nullptr;
    mNextActiveLevelName.clear();
}

void LevelManager::resizeLevel(int32_t width, int32_t height) {
    if (mActiveLevel) {
        mActiveLevel->onResize(width, height);
    }
}

} // namespace engine
