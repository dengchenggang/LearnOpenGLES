//
// Created by banma-3361 on 2025/11/4.
//

#include "CEGLSurface.h"
#include "Log.h"
#include <GLES3/gl3.h>
#include <limits>
#include <android/log.h>
#include <chrono>
#include "Engine.h"

KEY_VALUE(TAG, CEGLSurface)

CEGLSurface::CEGLSurface()
    : mTaskPool(std::make_unique<TaskPool>())
    , mDisplay(EGL_NO_DISPLAY)
    , mConfig(nullptr)
    , mContext(EGL_NO_CONTEXT)
    , mSurface(EGL_NO_SURFACE) {
}

CEGLSurface::~CEGLSurface() {
    release();
}

bool CEGLSurface::initialize(std::int32_t gles) {
    LogI("%s initialize enter.", TAG);

    if (mInitialized) {
        LogW("%s already initialized!", TAG);
        return true;
    }

    mGLESVersion = gles;

    // 启动 TaskPool
    mTaskPool->start();

    // 在 TaskPool 中执行 EGL 初始化
    auto future = mTaskPool->submit(&CEGLSurface::doInitialize, this, gles);
    bool result = future.get();  // 等待初始化完成

    LogI("%s initialize exit.", TAG);
    return result;
}

bool CEGLSurface::doInitialize(std::int32_t gles) {
    mDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (mDisplay == EGL_NO_DISPLAY) {
        LogE("eglGetDisplay failed");
        return false;
    }
    eglInitialize(mDisplay, nullptr, nullptr);
    const EGLint cfgAttr[] = {
            EGL_RENDERABLE_TYPE, gles >= 3 ? EGL_OPENGL_ES3_BIT : EGL_OPENGL_ES2_BIT,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE, 8,
            EGL_ALPHA_SIZE, 8, EGL_DEPTH_SIZE, 16,
            EGL_NONE
    };
    EGLint numCfg {std::numeric_limits<EGLint>::min()};
    eglChooseConfig(mDisplay, cfgAttr, &mConfig, 1, &numCfg);

    const EGLint ctxAttr[] = {EGL_CONTEXT_CLIENT_VERSION, gles, EGL_NONE};
    mContext = eglCreateContext(mDisplay, mConfig, nullptr, ctxAttr);
    if (mContext == EGL_NO_CONTEXT) {
        LogE("eglCreateContext failed!");
        return false;
    }

    EngineSingleton.init();
    mInitialized = true;
    LogI("%s EGL initialized successfully.", TAG);
    return true;
}

bool CEGLSurface::bind(ANativeWindow *window) {
    LogI("%s bind enter.", TAG);

    if (!mInitialized) {
        LogE("%s not initialized!", TAG);
        return false;
    }

    if (mRunning) {
        LogW("%s render loop already running!", TAG);
        return true;
    }

    // 在 TaskPool 中执行绑定
    auto future = mTaskPool->submit(&CEGLSurface::doBind, this, window);
    bool result = future.get();  // 等待绑定完成

    if (result) {
        // 启动渲染循环（通过 TaskPool 延时任务调度）
        mStopRequested = false;
        mRunning = true;
        mLastFrameTime = std::chrono::steady_clock::now();

        // 提交第一帧渲染任务
        mTaskPool->detach(&CEGLSurface::renderFrame, this);

        LogI("%s render loop started.", TAG);
    }

    LogI("%s bind exit.", TAG);
    return result;
}

bool CEGLSurface::doBind(ANativeWindow *window) {
    if (mSurface != EGL_NO_SURFACE) {
        LogE("mSurface already exists!");
        return false;
    }

    mSurface = eglCreateWindowSurface(mDisplay, mConfig, window, nullptr);

    if (mSurface == EGL_NO_SURFACE) {
        LogE("eglCreateWindowSurface failed!");
        return false;
    }

    if (!eglMakeCurrent(mDisplay, mSurface, mSurface, mContext)) {
        LogE("eglMakeCurrent failed!");
        return false;
    }

    eglQuerySurface(mDisplay, mSurface, EGL_WIDTH, &mWidth);
    eglQuerySurface(mDisplay, mSurface, EGL_HEIGHT, &mHeight);

    EngineSingleton.setBackground(1, 0, 0, 1);
    mBound = true;

    LogI("%s bind success: width=%d, height=%d.", TAG, mWidth, mHeight);
    return true;
}

void CEGLSurface::resize(std::int32_t w, std::int32_t h) {
    // 提交调整大小任务到 TaskPool
    mTaskPool->detach(&CEGLSurface::doResize, this, w, h);
}

void CEGLSurface::doResize(std::int32_t w, std::int32_t h) {
    LogI("%s resize enter, width=%d, height=%d", TAG, w, h);
    if (!mBound) {
        return;
    }

    mWidth = w;
    mHeight = h;

    EngineSingleton.setViewPort(w, h);
    LogI("%s resize exit.", TAG);
}

void CEGLSurface::setTargetFPS(int32_t fps) {
    if (fps > 0) {
        mTargetFrameIntervalMs = 1000 / fps;
        LogI("%s target FPS set to %d, interval=%ld ms", TAG, fps, mTargetFrameIntervalMs);
    }
}

void CEGLSurface::submit(std::function<void()> task) {
    if (!mTaskPool || !mTaskPool->isRunning()) {
        LogW("%s TaskPool not running, task ignored.", TAG);
        return;
    }
    mTaskPool->detach(std::move(task));
}

void CEGLSurface::submitDelayed(std::function<void()> task, int64_t delayMs) {
    if (!mTaskPool || !mTaskPool->isRunning()) {
        LogW("%s TaskPool not running, delayed task ignored.", TAG);
        return;
    }
    mTaskPool->detachDelayed(delayMs, std::move(task));
}

void CEGLSurface::renderFrame() {
    if (mStopRequested || !mRunning) {
        LogI("%s renderFrame stopped.", TAG);
        return;
    }

    auto frameStartTime = std::chrono::steady_clock::now();
    auto deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(frameStartTime - mLastFrameTime).count();
    mLastFrameTime = frameStartTime;

    // 记录 swapBuffers 耗时
    auto swapStart = std::chrono::steady_clock::now();
    eglSwapBuffers(mDisplay, mSurface);
    auto swapCost = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now() - swapStart).count();

    // 记录 update 耗时
    auto updateStart = std::chrono::steady_clock::now();
    EngineSingleton.update(deltaTime);
    auto updateCost = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now() - updateStart).count();

    // 记录 render 耗时
    auto renderStart = std::chrono::steady_clock::now();
    EngineSingleton.render(deltaTime);
    auto renderCost = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now() - renderStart).count();

    // 计算总帧耗时
    auto frameCost = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now() - frameStartTime).count();

    LogI("%s renderFrame: deltaTime=%lld ms, update=%lld us, render=%lld us, swap=%lld us, total=%lld us",
         TAG, deltaTime, updateCost, renderCost, swapCost, frameCost);

    // 调度下一帧（使用延时任务控制帧率）
    scheduleNextFrame();
}

void CEGLSurface::scheduleNextFrame() {
    if (mStopRequested || !mRunning) {
        return;
    }

    // 计算实际耗时，动态调整延时
    auto currentTime = std::chrono::steady_clock::now();
    auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - mLastFrameTime).count();
    auto delayMs = mTargetFrameIntervalMs - elapsedMs;

    if (delayMs < 0) {
        delayMs = 0;  // 如果已经超时，立即执行下一帧
    }
    // LogI("%s scheduleNextFrame: mTargetFrameIntervalMs=%ld ms, delayMs=%lld ms, elapsedMs=%lld ms", TAG, mTargetFrameIntervalMs, delayMs, elapsedMs);
    // 使用延时任务调度下一帧
    mTaskPool->detachDelayed(delayMs, &CEGLSurface::renderFrame, this);
}

void CEGLSurface::unbind() {
    LogI("%s unbind enter", TAG);

    // 停止渲染循环
    if (mRunning) {
        mStopRequested = true;
        mRunning = false;
        LogI("%s render loop stopping...", TAG);
    }

    // 提交解绑任务到 TaskPool
    auto future = mTaskPool->submit(&CEGLSurface::doUnbind, this);
    future.wait();  // 等待解绑完成

    LogI("%s unbind exit", TAG);
}

void CEGLSurface::doUnbind() {
    eglMakeCurrent(mDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if (mSurface != EGL_NO_SURFACE) {
        eglDestroySurface(mDisplay, mSurface);
        mSurface = EGL_NO_SURFACE;
    }
    mBound = false;
    LogI("%s unbind done", TAG);
}

void CEGLSurface::release() {
    LogI("%s release enter.", TAG);

    // 停止渲染循环
    if (mRunning) {
        mStopRequested = true;
        mRunning = false;
    }

    // 停止 TaskPool
    if (mTaskPool && mTaskPool->isRunning()) {
        // 提交释放任务到 TaskPool
        auto future = mTaskPool->submit(&CEGLSurface::doRelease, this);
        future.wait();  // 等待释放完成

        mTaskPool->stop();
    }

    mInitialized = false;
    mBound = false;

    LogI("%s release exit.", TAG);
}

void CEGLSurface::doRelease() {
    EngineSingleton.release();
    if (mDisplay != EGL_NO_DISPLAY) {
        eglMakeCurrent(mDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (mSurface != EGL_NO_SURFACE) {
            eglDestroySurface(mDisplay, mSurface);
            mSurface = EGL_NO_SURFACE;
        }
        if (mContext != EGL_NO_CONTEXT) {
            eglDestroyContext(mDisplay, mContext);
            mContext = EGL_NO_CONTEXT;
        }
        eglTerminate(mDisplay);
        mDisplay = EGL_NO_DISPLAY;
    }
    LogI("%s doRelease done.", TAG);
}
