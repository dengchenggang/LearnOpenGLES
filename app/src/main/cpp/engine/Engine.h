#ifndef C_ENGINE_PROXY_H
#define C_ENGINE_PROXY_H
#include <cstdint>
#include <memory>
#include <vector>
#include <map>
#include <string>
#include <atomic>
#include <chrono>
#include "Camera.h"
#include "level/Level.h"
#include "level/ILevelManager.h"
#include <common/Singleton.hpp>
#include "OpenGLESRenderContext.h"
#include "utils/TaskPool.h"

namespace engine {

class Engine : public ILevelManager {
    friend class framework::Singleton<Engine>;
public:
    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;
public:
    void init(std::int32_t gles, std::map<std::string, LevelPtr>&& levels, const std::string& startLevel);
    void attachSurface(ANativeWindow* window);
    void resize(std::int32_t w, std::int32_t h);
    void detachSurface();
    void deInit();
public:
    Camera& getCamera() { return mCamera; }
    void changeLevel(const std::string& levelName) override;
private:
    Engine();
    ~Engine();
private:
    void applyLevelViewport();
    void renderFrame();
    void update(int64_t deltaTime);
    void render(int64_t deltaTime);
    void doChangeLevel(const std::string& levelName);
private:
    std::atomic_bool mRenderLoopStarted;
    Camera mCamera;
    std::map<std::string, LevelPtr> mLevels;
    Level* mActiveLevel;
    std::string mNextActiveLevelName;
    std::string mPreviousActiveLevelName;
private:
    renderer::OpenGLESRenderContext mRenderContext {};
    std::unique_ptr<TaskPool> mTaskPool {std::make_unique<TaskPool>()};

    std::atomic<bool> mRunning{false};
    std::atomic<bool> mStopRequested{false};

    std::int64_t mTargetFrameIntervalMs{33};
    std::chrono::steady_clock::time_point mLastFrameTime;
};

} // namespace engine

#define Engine framework::Singleton<engine::Engine>::getInstance()
#endif
