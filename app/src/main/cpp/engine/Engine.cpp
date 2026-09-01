#include "Engine.h"

#undef Engine

#include "RenderInterface.h"
#include "filesystem/FileSystem.h"
#include "utils/Log.h"

constexpr const char* TAG {"Engine"};
constexpr const char* BACKGROUD_PATH = "textures/background.png";

namespace engine {

void Engine::init() {
}

void Engine::setBackground(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    RenderInterface.setClearColor(r, g, b, a);
}

void Engine::setViewPort(int32_t width, int32_t height) {
    RenderInterface.setViewport(0, 0, width, height);
    bool ret = FileSystem.exists(BACKGROUD_PATH);
    LogI("%s setViewPort: %s exists %d", TAG, BACKGROUD_PATH, ret);
}

void Engine::update(int64_t deltaTime) {
    for (auto& actor : mActors) {
        if (actor->isActive()) {
            actor->update(deltaTime);
        }
    }
}

void Engine::render(int64_t deltaTime) {
    RenderInterface.clear(true, true, false);
    for (auto& actor : mActors) {
        if (actor->isActive()) {
            actor->render();
        }
    }
}

void Engine::release() {
    mActors.clear();
}

Actor& Engine::createActor() {
    mActors.emplace_back(new Actor());
    return *mActors.back();
}

} // namespace engine
