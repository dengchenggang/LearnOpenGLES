#include "Engine.h"
#include "RenderInterface.h"
#include "FileSystem.h"
#include "Log.h"

constexpr const char* TAG {"Engine"};
constexpr const char* BACKGROUD_PATH = "textures/background.png";

void EngineProxy::init() {
}

void EngineProxy::setBackground(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    RenderInterface.setClearColor(r, g, b, a);
}

void EngineProxy::setViewPort(int32_t width, int32_t height) {
    RenderInterface.setViewport(0, 0, width, height);
    bool ret = FileSystem.exists(BACKGROUD_PATH);
    LogI("%s setViewPort: %s exists %d", TAG, BACKGROUD_PATH, ret);
}

void EngineProxy::update(int64_t deltaTime) {

}

void EngineProxy::render(int64_t deltaTime) {
    RenderInterface.clear(true, true, false);
}

void EngineProxy::release() {

}