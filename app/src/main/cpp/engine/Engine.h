#ifndef C_ENGINE_PROXY_H
#define C_ENGINE_PROXY_H
#include <cstdint>
#include <memory>
#include <vector>
#include "Actor.h"
#include <common/Singleton.hpp>

namespace engine {

class Engine {
    friend class Singleton<Engine>;
public:
    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;
public:
    void init();
    void setViewPort(int32_t width, int32_t height);
    void setBackground(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    void update(int64_t deltaTime);
    void render(int64_t deltaTime);
    void release();

    Actor& createActor();

    template<typename Func>
    void forEachActor(Func&& func) {
        for (auto& actor : mActors) {
            func(*actor);
        }
    }

    template<typename Func>
    void forEachActor(Func&& func) const {
        for (const auto& actor : mActors) {
            func(*actor);
        }
    }
private:
    Engine() = default;
    ~Engine() = default;
private:
    std::vector<ActorPtr> mActors;
};

} // namespace engine

#define Engine Singleton<engine::Engine>::getInstance()
#endif
