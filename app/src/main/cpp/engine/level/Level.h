#ifndef LEVEL_H
#define LEVEL_H

#include <vector>
#include <memory>
#include "Actor.h"

namespace engine {
class ILevelManager;

class Level {
public:
    Level(const std::string& name, ILevelManager& levelManager);
    virtual ~Level();
    Level(const Level&) = delete;
    Level& operator=(const Level&) = delete;
public:
    const std::string& getId() const { return mName; }
    virtual void init() = 0;
    void beginPlay();
    void update(int64_t deltaTime);
    void render();
    void endPlay();
    virtual void deInit() = 0;
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