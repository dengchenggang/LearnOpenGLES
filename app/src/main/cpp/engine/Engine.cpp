#include "Engine.h"

#undef Engine

#include "RenderInterface.h"
#include "filesystem/FileSystem.h"
#include "utils/Log.h"

constexpr const char* TAG {"Engine"};
constexpr const char* BACKGROUD_PATH = "textures/background.png";

namespace engine {

Engine::Engine() = default;

Engine::~Engine() {
    deInit();
}

void Engine::init(std::int32_t gles, std::map<std::string, LevelPtr>&& levels) {
    mTaskPool->start();
    auto future = mTaskPool->submit([this, gles, levels = std::move(levels)]() mutable {
        bool result = mRenderContext.initialize(gles);
        if (result) {
            mActiveLevel = nullptr;
            mNextActiveLevelName.clear();
            mLevels.clear();

            for (auto& pair : levels) {
                if (pair.second) {
                    mLevels[pair.first] = std::move(pair.second);
                }
            }

            for (auto& pair : mLevels) {
                pair.second->init();
            }
        }
        return result;
    });
    future.get();
}

void Engine::beginPlay(ANativeWindow* window) {
    LogI("%s beginPlay enter.", TAG);

    if (!mRenderContext.isInitialized()) {
        LogE("%s EGL not initialized!", TAG);
        return;
    }

    if (mRunning) {
        LogW("%s render loop already running!", TAG);
        return;
    }

    auto future = mTaskPool->submit([this, window]() {
        bool result = mRenderContext.beginPlay(window);
        if (result) {
            RenderInterface.setClearColor(1, 0, 0, 1);
        }
        return result;
    });
    bool result = future.get();

    if (result) {
        mStopRequested = false;
        mRunning = true;
        mLastFrameTime = std::chrono::steady_clock::now();
        mTaskPool->detach([this]() { renderFrame(); });
        LogI("%s render loop started.", TAG);
    }

    LogI("%s beginPlay exit.", TAG);
}

void Engine::resize(std::int32_t w, std::int32_t h) {
    mTaskPool->detach([this, w, h]() {
        LogI("%s resize enter, width=%d, height=%d", TAG, w, h);
        if (!mRenderContext.isBound()) {
            return;
        }
        mRenderContext.setViewPort(w, h);
        RenderInterface.setViewport(0, 0, w, h);
        LogI("%s resize exit.", TAG);
    });
}

void Engine::endPlay() {
    LogI("%s endPlay enter", TAG);

    if (mRunning) {
        mStopRequested = true;
        mRunning = false;
        LogI("%s render loop stopping...", TAG);
    }

    auto future = mTaskPool->submit([this]() {
        mRenderContext.endPlay();
    });
    future.wait();

    LogI("%s endPlay exit", TAG);
}

void Engine::deInit() {
    if (mRunning) {
        mStopRequested = true;
        mRunning = false;
    }

    if (mTaskPool && mTaskPool->isRunning()) {
        auto future = mTaskPool->submit([this]() {
            mActiveLevel = nullptr;
            mNextActiveLevelName = "";
            mRenderContext.deInitialize();
        });
        future.wait();
        mTaskPool->stop();
    } else {
        mActiveLevel = nullptr;
        mNextActiveLevelName = "";
        mRenderContext.deInitialize();
    }
}

void Engine::changeLevel(const std::string& levelName) {
    LogI("changeLevel: %s --> %s", mActiveLevel ? mActiveLevel->getId().c_str() : "nullptr", levelName.c_str());
    mNextActiveLevelName = levelName;
}

void Engine::renderFrame() {
    if (mStopRequested || !mRunning) {
        LogI("%s renderFrame stopped.", TAG);
        return;
    }

    auto frameStartTime = std::chrono::steady_clock::now();
    auto deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(frameStartTime - mLastFrameTime).count();
    mLastFrameTime = frameStartTime;

    auto swapStart = std::chrono::steady_clock::now();
    mRenderContext.swapBuffers();
    auto swapCost = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now() - swapStart).count();

    auto updateStart = std::chrono::steady_clock::now();
    update(deltaTime);
    auto updateCost = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now() - updateStart).count();

    auto renderStart = std::chrono::steady_clock::now();
    render(deltaTime);
    auto renderCost = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now() - renderStart).count();

    auto frameCost = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now() - frameStartTime).count();

    LogI("%s renderFrame: deltaTime=%lld ms, update=%lld us, render=%lld us, swap=%lld us, total=%lld us",
         TAG, deltaTime, updateCost, renderCost, swapCost, frameCost);

    scheduleNextFrame();
}

void Engine::scheduleNextFrame() {
    if (mStopRequested || !mRunning) {
        return;
    }

    auto currentTime = std::chrono::steady_clock::now();
    auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - mLastFrameTime).count();
    auto delayMs = mTargetFrameIntervalMs - elapsedMs;

    if (delayMs < 0) {
        delayMs = 0;
    }

    mTaskPool->detachDelayed(delayMs, [this]() { renderFrame(); });
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
