#include "ViewManager.h"
#include "utils/Log.h"
#include "Engine.h"

#undef ViewManager

constexpr const char* TAG {"ViewManager"};

namespace view {

void ViewManager::init(std::int32_t gles) {
    mTaskPool->start();
    auto future = mTaskPool->submit([this, gles]() {
        bool result = mEGLSurface.initialize(gles);
        if (result) {
            Engine.init();
        }
        return result;
    });
    future.get();
}

void ViewManager::bind(ANativeWindow *window) {
    LogI("%s bind enter.", TAG);

    if (!mEGLSurface.isInitialized()) {
        LogE("%s EGL not initialized!", TAG);
        return;
    }

    if (mRunning) {
        LogW("%s render loop already running!", TAG);
        return;
    }

    auto future = mTaskPool->submit([this, window]() {
        bool result = mEGLSurface.bind(window);
        if (result) {
            Engine.setBackground(1, 0, 0, 1);
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

    LogI("%s bind exit.", TAG);
}

void ViewManager::renderFrame() {
    if (mStopRequested || !mRunning) {
        LogI("%s renderFrame stopped.", TAG);
        return;
    }

    auto frameStartTime = std::chrono::steady_clock::now();
    auto deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(frameStartTime - mLastFrameTime).count();
    mLastFrameTime = frameStartTime;

    auto swapStart = std::chrono::steady_clock::now();
    mEGLSurface.swapBuffers();
    auto swapCost = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now() - swapStart).count();

    auto updateStart = std::chrono::steady_clock::now();
    Engine.update(deltaTime);
    auto updateCost = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now() - updateStart).count();

    auto renderStart = std::chrono::steady_clock::now();
    Engine.render(deltaTime);
    auto renderCost = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now() - renderStart).count();

    auto frameCost = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now() - frameStartTime).count();

    LogI("%s renderFrame: deltaTime=%lld ms, update=%lld us, render=%lld us, swap=%lld us, total=%lld us",
         TAG, deltaTime, updateCost, renderCost, swapCost, frameCost);

    scheduleNextFrame();
}

void ViewManager::scheduleNextFrame() {
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

void ViewManager::resize(std::int32_t w, std::int32_t h) {
    mTaskPool->detach([this, w, h]() {
        LogI("%s resize enter, width=%d, height=%d", TAG, w, h);
        if (!mEGLSurface.isBound()) {
            return;
        }
        mEGLSurface.setViewPort(w, h);
        Engine.setViewPort(w, h);
        LogI("%s resize exit.", TAG);
    });
}

void ViewManager::unbind() {
    LogI("%s unbind enter", TAG);

    if (mRunning) {
        mStopRequested = true;
        mRunning = false;
        LogI("%s render loop stopping...", TAG);
    }

    auto future = mTaskPool->submit([this]() {
        mEGLSurface.unbind();
    });
    future.wait();

    LogI("%s unbind exit", TAG);
}

void ViewManager::destroy() {
    LogI("%s destroy enter.", TAG);

    if (mRunning) {
        mStopRequested = true;
        mRunning = false;
    }

    if (mTaskPool && mTaskPool->isRunning()) {
        auto future = mTaskPool->submit([this]() {
            Engine.release();
            mEGLSurface.release();
        });
        future.wait();
        mTaskPool->stop();
    }

    LogI("%s destroy exit.", TAG);
}

} // namespace view
