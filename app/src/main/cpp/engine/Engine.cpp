#include "Engine.h"

#undef Engine

#include "RenderInterface.h"
#include "filesystem/FileSystem.h"
#include "level/NilLevel.h"
#include "utils/Log.h"

namespace engine {

Engine::Engine()
    : mRenderLoopStarted(false)
{

}

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
            mRenderContext.attachOffscreenSurface();
        }
        if (result) {
            initLevels(std::move(levels), mCamera);
            setViewportSize(1, 1);
            changeLevel(startLevel);

            // 在 1x1 pbuffer 上空跑一帧，逼驱动提前完成 shader 的最终编译
            update(0);
            render(0);
            RenderInterface.finish();
        }
        LOG_EXIT(": result=%d", result);
        return result;
    });
    future.get();
}

void Engine::attachSurface(ANativeWindow* window) {
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
        bool result = mRenderContext.attachSurface(window);
        if (result) {
            RenderInterface.setClearColor(0, 0, 0, 0);
        }
        return result;
    });
    bool result = future.get();

    if (result) {
        mStopRequested = false;
        mRunning = true;
        if (!mPreviousActiveLevelName.empty()) {
            changeLevel(mPreviousActiveLevelName);
            mPreviousActiveLevelName.clear();
        }
        LogI("render loop armed, waiting for first resize.");
    }

    LOG_EXIT(".");
}

void Engine::resize(std::int32_t w, std::int32_t h) {
    mTaskPool->detach([this, w, h]() {
        LOG_ENTER(": width=%d, height=%d", w, h);
        if (!mRenderContext.isBound()) {
            return;
        }
        setViewportSize(w, h);
        resizeLevel(w, h);
        LOG_EXIT(".");
        if (mRunning && !mRenderLoopStarted.exchange(true)) {
            LogI("render loop started.");
            mLastFrameTime = std::chrono::steady_clock::now();
            renderFrame();
        }
    });
}

void Engine::detachSurface() {
    LOG_ENTER(".");

    if (mRunning) {
        mStopRequested = true;
        mRunning = false;
        LogI("render loop stopping...");
    }
    mRenderLoopStarted = false;

    auto future = mTaskPool->submit([this]() {
        doChangeLevel(NilLevel::NAME);
        mRenderContext.detachSurface();
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
            deInitLevels();
            mRenderContext.deInitialize();
        });
        future.wait();
        mTaskPool->stop();
    } else {
        deInitLevels();
        mRenderContext.deInitialize();
    }
}

void Engine::onLogicalViewportChanged(const glm::vec4& rect) {
    mCamera.setViewport(rect.x, rect.y, rect.z, rect.w);
}

void Engine::onRenderViewportChanged(const glm::vec4& rect) {
    if (!mRenderContext.isBound()) {
        return;
    }
    mRenderContext.setViewPort(static_cast<int32_t>(rect.x), static_cast<int32_t>(rect.y),
                               static_cast<int32_t>(rect.z), static_cast<int32_t>(rect.w));
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
    updateLevels(deltaTime);
}

void Engine::render(int64_t deltaTime) {
    RenderInterface.clear(true, true, false);
    renderLevels();
}

} // namespace engine
