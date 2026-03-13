//
// Created by banma-3361 on 2025/11/4.
//

#ifndef LEARNOPENGLES_CEGLSURFACE_H
#define LEARNOPENGLES_CEGLSURFACE_H
#include <EGL/egl.h>
#include <android/native_window_jni.h>
#include <cstdint>
#include <memory>
#include <atomic>
#include <chrono>
#include "IRenderInterface.h"
#include "TaskPool.h"

class CEGLSurface {
private:
    std::unique_ptr<IRenderInterface> mRenderInterface;
    std::unique_ptr<TaskPool> mTaskPool;

    EGLDisplay mDisplay;
    EGLConfig mConfig;
    EGLContext mContext;
    EGLSurface mSurface;

    // 尺寸状态（单线程访问，无需加锁）
    std::int32_t mWidth{0};
    std::int32_t mHeight{0};

    // 渲染循环控制
    std::atomic<bool> mInitialized{false};
    std::atomic<bool> mBound{false};
    std::atomic<bool> mRunning{false};
    std::atomic<bool> mStopRequested{false};
    std::int32_t mGLESVersion{3};

    // 渲染帧率控制
    std::int64_t mTargetFrameIntervalMs{33};  // 默认约 60 FPS
    std::chrono::steady_clock::time_point mLastFrameTime;

public:
    explicit CEGLSurface(std::unique_ptr<IRenderInterface>&& renderInterface);
    ~CEGLSurface();

    CEGLSurface(const CEGLSurface&) = delete;
    CEGLSurface& operator =(const CEGLSurface&) = delete;

public:
    // 在主线程调用

    bool initialize(std::int32_t gles);
    bool bind(ANativeWindow *window);
    void resize(std::int32_t w, std::int32_t h);
    void unbind();
    void release();

    // 设置目标帧率（FPS）
    void setTargetFPS(int32_t fps);

    // 检查状态
    bool isInitialized() const { return mInitialized; }
    bool isBound() const { return mBound; }
    bool isRunning() const { return mRunning; }

private:
    // 提交任务到 TaskPool 执行（内部使用）
    void submit(std::function<void()> task);

    // 提交延时任务到 TaskPool（内部使用）
    void submitDelayed(std::function<void()> task, int64_t delayMs);

    // 渲染一帧（在 TaskPool 中执行）
    void renderFrame();

    // 调度下一帧（使用延时任务控制帧率）
    void scheduleNextFrame();

    // 在 TaskPool 线程执行的方法
    bool doInitialize(std::int32_t gles);
    bool doBind(ANativeWindow *window);
    void doResize(std::int32_t w, std::int32_t h);
    void doUnbind();
    void doRelease();
};


#endif //LEARNOPENGLES_CEGLSURFACE_H
