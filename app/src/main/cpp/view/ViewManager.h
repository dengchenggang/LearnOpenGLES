#ifndef VIEW_MANAGER_H
#define VIEW_MANAGER_H

#include "common/Singleton.hpp"
#include "ViewCtrlEGLSurface.h"
#include "utils/TaskPool.h"
#include <cstdint>
#include <string>
#include <memory>
#include <atomic>
#include <chrono>


namespace view {

class ViewManager {
    friend class Singleton<ViewManager>;
public:
    ViewManager(const ViewManager&) = delete;
    ViewManager& operator= (const ViewManager&) = delete;
public:
    void init(std::int32_t gles);
    void bind(ANativeWindow *window);
    void resize(std::int32_t w, std::int32_t h);
    void unbind();
    void destroy();

private:
    ViewManager() = default;
    ~ViewManager() = default;

    void renderFrame();
    void scheduleNextFrame();

private:
    ViewCtrlEGLSurface mEGLSurface {};
    std::unique_ptr<TaskPool> mTaskPool {std::make_unique<TaskPool>()};

    std::atomic<bool> mRunning{false};
    std::atomic<bool> mStopRequested{false};

    std::int64_t mTargetFrameIntervalMs{33};
    std::chrono::steady_clock::time_point mLastFrameTime;
};

} // namespace view

#define ViewManager Singleton<view::ViewManager>::getInstance()

#endif
