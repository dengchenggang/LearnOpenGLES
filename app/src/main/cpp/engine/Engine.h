#ifndef C_ENGINE_PROXY_H
#define C_ENGINE_PROXY_H
#include <cstdint>
#include <memory>
#include <vector>
#include <map>
#include <string>
#include "level/Level.h"
#include "level/ILevelManager.h"
#include <common/Singleton.hpp>

namespace engine {

class Engine : public ILevelManager {
    friend class framework::Singleton<Engine>;
public:
    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;
public:
    void init(std::vector<LevelPtr>& levels, const std::string& startLevel);
    void setViewPort(int32_t width, int32_t height);
    void setBackground(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    void update(int64_t deltaTime);
    void render(int64_t deltaTime);
    void deInit();
public:
    void changeLevel(const std::string& levelName) override;
private:
    Engine() = default;
    ~Engine() = default;
    void doChangeLevel(const std::string& levelName);
private:
    std::map<std::string, LevelPtr> mLevels;
    Level* mActiveLevel;
    std::string mNextActiveLevelName;
};

} // namespace engine

#define Engine framework::Singleton<engine::Engine>::getInstance()
#endif
