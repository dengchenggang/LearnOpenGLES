#include "Engine.h"

#undef Engine

#include "RenderInterface.h"
#include "filesystem/FileSystem.h"
#include "utils/Log.h"

constexpr const char* TAG {"Engine"};
constexpr const char* BACKGROUD_PATH = "textures/background.png";

namespace engine {

void Engine::init(std::vector<LevelPtr>& levels, const std::string& startLevel) {
    mActiveLevel = nullptr;
    mNextActiveLevelName = startLevel;
    mLevels.clear();

    for (auto& level : levels) {
        if (level) {
            mLevels[level->getId()] = std::move(level);
        }
    }

    for (auto& pair : mLevels) {
        pair.second->init();
    }
}

void Engine::setBackground(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    RenderInterface.setClearColor(r, g, b, a);
}

void Engine::setViewPort(int32_t width, int32_t height) {
    RenderInterface.setViewport(0, 0, width, height);
    bool ret = FileSystem.exists(BACKGROUD_PATH);
    LogI("%s setViewPort: %s exists %d", TAG, BACKGROUD_PATH, ret);
}

void Engine::update(int64_t deltaTime) {
    if (!mNextActiveLevelName.empty()) {
        doChangeLevel(mNextActiveLevelName);
    }

    if (mActiveLevel) {
        mActiveLevel->update(deltaTime);
    }
}

void Engine::render(int64_t deltaTime) {
    RenderInterface.clear(true, true, false);
    if (mActiveLevel) {
        mActiveLevel->render();
    }
}

void Engine::deInit() {
    mActiveLevel = nullptr;
    mNextActiveLevelName = "";
}

void Engine::changeLevel(const std::string& levelName) {
    LogI("changeLevel: %s --> %s", mActiveLevel ? mActiveLevel->getId().c_str() : "nullptr", levelName.c_str());
    mNextActiveLevelName = levelName;
}

void Engine::doChangeLevel(const std::string& levelName) {
    LogI("doChangeLevel: %s --> %s", mActiveLevel ? mActiveLevel->getId().c_str() : "nullptr", levelName.c_str());
    auto it = mLevels.find(levelName);
    if (it != mLevels.end()) {
        auto oldLevel = mActiveLevel;
        mActiveLevel = it->second.get();
        if (oldLevel) {
            oldLevel->endPlay();
        }
        if (mActiveLevel) {
            mActiveLevel->beginPlay();
        }
    } else {
        LogE("doChangeLevel: %s not found", levelName.c_str());
    }
    mNextActiveLevelName = "";
}

} // namespace engine