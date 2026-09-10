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
    virtual void onBeginPlay();
    virtual void onUpdate(int64_t deltaTime);
    virtual void onRender();
    virtual void onEndPlay();
    virtual void onDeInit() {}
protected:
    Actor& createActor();
    void changeLevel(const std::string& levelName);

private:
    ILevelManager& mLevelManager;
    std::string mName;
    std::vector<ActorPtr> mActors;
};

using LevelPtr = std::unique_ptr<Level>;

} // namespace engine

#endif // LEVEL_H