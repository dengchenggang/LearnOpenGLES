#include "Engine.h"
#include "RenderInterface.h"

void Engine::init() {
}

void Engine::setBackground(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    RenderInterface.setClearColor(r, g, b, a);
}

void Engine::setViewPort(int32_t width, int height) {
    RenderInterface.setViewport(0, 0, width, height);
}

void Engine::update(int64_t deltaTime) {

}

void Engine::render(int64_t deltaTime) {
    RenderInterface.clear(true, true, false);
}

void Engine::release() {

}