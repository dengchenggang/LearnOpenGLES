#ifndef LEVEL_H
#define LEVEL_H

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "Actor.h"

namespace engine {
class ILevelManager;
class Camera;

// 设计尺寸与实际视口宽高比不一致时的适配策略
enum class ScaleMode {
    Fit,    // 等比缩放，设计区域完整可见，视口两侧留黑边
    Fill,   // 等比铺满视口，设计区域超出部分被裁剪
    Expand, // 固定较短的一边，另一边按视口比例扩展逻辑尺寸，无黑边无裁剪
};

class Level {
    friend class Engine;
public:
    Level(const std::string& name, ILevelManager& levelManager, Camera& camera,
          const glm::vec2& designSize, ScaleMode scaleMode = ScaleMode::Fit);
    virtual ~Level();
    Level(const Level&) = delete;
    Level& operator=(const Level&) = delete;
public:
    const std::string& getId() const { return mName; }
    Camera& getCamera() const { return mCamera; }

    // Actor 布局所基于的固定设计尺寸
    const glm::vec2& getDesignSize() const { return mDesignSize; }
    // 适配后的逻辑矩形 (left, bottom, width, height)，即相机正交投影的范围
    // Fit/Fill 下恒为 (0, 0, 设计宽, 设计高)；Expand 下扩展的一边向两侧均分，使设计区域保持居中
    const glm::vec4& getLogicalRect() const { return mLogicalRect; }
    // 适配后的渲染输出矩形 (x, y, width, height)，单位为物理像素
    // Fit/Fill 下可能不等于物理视口（等比缩放后居中）；Expand 下与物理视口一致
    const glm::vec4& getRenderRect() const { return mRenderRect; }

    ScaleMode getScaleMode() const { return mScaleMode; }
    void setScaleMode(ScaleMode scaleMode);

    template<typename T = Actor, typename... Args>
    T& createActor(Args&&... args) {
        static_assert(std::is_base_of_v<Actor, T>, "T must derive from Actor");
        auto actor = std::unique_ptr<T>(new T(*this, sNextActorId++, std::forward<Args>(args)...));
        T* ptr = actor.get();
        mActors.push_back(std::move(actor));
        if (mBegunPlay) {
            ptr->onBeginPlay();
        }
        return *ptr;
    }
    void destroyActor(uint64_t actorId);
protected:
    void changeLevel(const std::string& levelName);
protected:
    virtual void onBeginPlay();
    virtual void onResize(int32_t width, int32_t height);
    virtual void onEnter(int32_t width, int32_t height);
    virtual void onUpdate(int64_t deltaTime);
    virtual void onRender();
    virtual void onExit() = 0;
    virtual void onEndPlay();
private:
    void updateAdaption(int32_t width, int32_t height);
private:
    static uint64_t sNextActorId;
    bool mBegunPlay = false;
    ILevelManager& mLevelManager;
    Camera& mCamera;
    std::string mName;
    std::vector<ActorPtr> mActors;

    glm::vec2 mDesignSize;
    ScaleMode mScaleMode;
    glm::vec4 mLogicalRect;
    glm::vec4 mRenderRect;
};

using LevelPtr = std::unique_ptr<Level>;

} // namespace engine

#endif // LEVEL_H