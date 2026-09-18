#ifndef LEVEL_H
#define LEVEL_H

#include <vector>
#include <memory>
#include "Actor.h"

namespace engine {
class ILevelManager;

class Level {
    friend class Engine;
public:
    Level(const std::string& name, ILevelManager& levelManager);
    virtual ~Level();
    Level(const Level&) = delete;
    Level& operator=(const Level&) = delete;
public:
    const std::string& getId() const { return mName; }
protected:
    virtual void onInit() = 0;
    virtual void onResize(int32_t width, int32_t height) {}
    virtual void onBeginPlay();
    virtual void onUpdate(int64_t deltaTime);
    virtual void onRender();
    virtual void onEndPlay();
    virtual void onDeInit() {}
protected:
    template<typename T = Actor, typename... Args>
    T& createActor(Args&&... args) {
        static_assert(std::is_base_of_v<Actor, T>, "T must derive from Actor");
        auto actor = std::unique_ptr<T>(new T(*this, std::forward<Args>(args)...));
        T* ptr = actor.get();
        mActors.push_back(std::move(actor));
        return *ptr;
    }
    void changeLevel(const std::string& levelName);
private:
    ILevelManager& mLevelManager;
    std::string mName;
    std::vector<ActorPtr> mActors;
};

using LevelPtr = std::unique_ptr<Level>;

} // namespace engine

#endif // LEVEL_H