#ifndef LEVEL_H
#define LEVEL_H

#include <vector>
#include <memory>
#include "Actor.h"

namespace engine {
class ILevelManager;
class Camera;

class Level {
    friend class Engine;
public:
    Level(const std::string& name, ILevelManager& levelManager, Camera& camera);
    virtual ~Level();
    Level(const Level&) = delete;
    Level& operator=(const Level&) = delete;
public:
    const std::string& getId() const { return mName; }
    Camera& getCamera() const { return mCamera; }

    template<typename T = Actor, typename... Args>
    T& createActor(Args&&... args) {
        static_assert(std::is_base_of_v<Actor, T>, "T must derive from Actor");
        auto actor = std::unique_ptr<T>(new T(*this, sNextActorId++, std::forward<Args>(args)...));
        T* ptr = actor.get();
        mActors.push_back(std::move(actor));
        return *ptr;
    }
    void destroyActor(uint64_t actorId);
protected:
    void changeLevel(const std::string& levelName);
protected:
    virtual void onBeginPlay() = 0;
    virtual void onResize(int32_t width, int32_t height) = 0;
    virtual void onEnter() = 0;
    virtual void onUpdate(int64_t deltaTime);
    virtual void onRender();
    virtual void onExit() = 0;
    virtual void onEndPlay() = 0;
private:
    static uint64_t sNextActorId;
    ILevelManager& mLevelManager;
    Camera& mCamera;
    std::string mName;
    std::vector<ActorPtr> mActors;
};

using LevelPtr = std::unique_ptr<Level>;

} // namespace engine

#endif // LEVEL_H