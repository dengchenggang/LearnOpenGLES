#include "Engine.h"
#include "RenderInterface.h"

void Engine::init() {
}

void Engine::setBackground(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    RenderInterface.setClearColor(r, g, b, a);
}

void Engine::start() {

}

void Engine::stop() {
}

void Engine::release() {

}