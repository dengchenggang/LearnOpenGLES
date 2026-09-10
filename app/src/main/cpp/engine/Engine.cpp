#include "Engine.h"

#undef Engine

#include "RenderInterface.h"
#include "filesystem/FileSystem.h"
#include "utils/Log.h"

namespace engine {

Engine::Engine() = default;

Engine::~Engine() {
    deInit();
}

void Engine::init(std::int32_t gles, std::map<std::string, LevelPtr>&& levels, const std::string& startLevel) {
    LogI("tartLevel=%s", startLevel.c_str());
    mTaskPool->start();
    auto future = mTaskPool->submit([this, gles, levels = std::move(levels), startLevel]() mutable {
        LOG_ENTER(": task startLevel=%s", startLevel.c_str());
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
                pair.second->onInit();
            }

            changeLevel(startLevel);
        }
        LOG_EXIT(": result=%d", result);
        return result;
    });
    future.get();
}

void Engine::beginPlay(ANativeWindow* window) {
    LOG_ENTER(".");

    if (!mRenderContext.isInitialized()) {
        LogE("EGL not initialized!");
        return;
    }

    if (mRunning) {
        LogW("render loop already running!");
        return;
    }

    auto future = mTaskPool->submit([this, window]() {
        bool result = mRenderContext.beginPlay(window);
        if (result) {
            RenderInterface.setClearColor(0, 0, 0, 0);
        }
        return result;
    });
    bool result = future.get();

    if (result) {
        mStopRequested = false;
        mRunning = true;
        mLastFrameTime = std::chrono::steady_clock::now();
        mTaskPool->detach([this]() { renderFrame(); });
        LogI("render loop started.");
    }

    LOG_EXIT(".");
}

void Engine::resize(std::int32_t w, std::int32_t h) {
    mTaskPool->detach([this, w, h]() {
        LOG_ENTER(": width=%d, height=%d", w, h);
        if (!mRenderContext.isBound()) {
            return;
        }
        mRenderContext.setViewPort(w, h);
        RenderInterface.setViewport(0, 0, w, h);
        LOG_EXIT(".");
    });
}

void Engine::endPlay() {
    LOG_ENTER(".");

    if (mRunning) {
        mStopRequested = true;
        mRunning = false;
        LogI("render loop stopping...");
    }

    auto future = mTaskPool->submit([this]() {
        mRenderContext.endPlay();
    });
    future.wait();

    LOG_EXIT(".");
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
    LogI("%s --> %s", mActiveLevel ? mActiveLevel->getId().c_str() : "nullptr", levelName.c_str());
    mNextActiveLevelName = levelName;
}

void Engine::renderFrame() {
    if (mStopRequested || !mRunning) {
        LOG_EXIT(": mStopRequested=%d, mRunning=%d", mStopRequested.load(), mRunning.load());
        return;
    }

    auto frameStartTime = std::chrono::steady_clock::now();
    auto deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(frameStartTime - mLastFrameTime).count();
    mLastFrameTime = frameStartTime;

    auto updateStart = std::chrono::steady_clock::now();
    update(deltaTime);
    auto updateCost = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now() - updateStart).count();

    auto renderStart = std::chrono::steady_clock::now();
    render(deltaTime);
    auto renderCost = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now() - renderStart).count();

    auto swapStart = std::chrono::steady_clock::now();
    mRenderContext.swapBuffers();
    auto swapCost = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now() - swapStart).count();

    auto fps = 1000.0f / deltaTime;
    auto currentTime = std::chrono::steady_clock::now();
    auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - mLastFrameTime).count();
    auto delayMs = mTargetFrameIntervalMs - elapsedMs;
    if (delayMs < 0) {
        delayMs = 0;
    }

    mTaskPool->detachDelayed(delayMs, [this]() { renderFrame(); });
    LogI("fps=%f, update=%lld us, render=%lld us, swap=%lld us, total=%lld ms, delay=%lld ms",
         fps, updateCost, renderCost, swapCost, elapsedMs, delayMs);
}

void Engine::update(int64_t deltaTime) {
    if (!mNextActiveLevelName.empty()) {
        doChangeLevel(mNextActiveLevelName);
    }

    if (mActiveLevel) {
        mActiveLevel->onUpdate(deltaTime);
    }
}

void Engine::render(int64_t deltaTime) {
    RenderInterface.clear(true, true, false);
    if (mActiveLevel) {
        mActiveLevel->onRender();
    }
}

void Engine::doChangeLevel(const std::string& levelName) {
    LogI("doChangeLevel: %s --> %s", mActiveLevel ? mActiveLevel->getId().c_str() : "nullptr", levelName.c_str());
    auto it = mLevels.find(levelName);
    if (it != mLevels.end()) {
        auto oldLevel = mActiveLevel;
        mActiveLevel = it->second.get();
        if (oldLevel) {
            oldLevel->onEndPlay();
        }
        if (mActiveLevel) {
            mActiveLevel->onBeginPlay();
        }
    } else {
        LogE("doChangeLevel: %s not found", levelName.c_str());
    }
    mNextActiveLevelName = "";
}

} // namespace engine
