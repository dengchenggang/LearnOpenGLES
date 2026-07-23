#ifndef C_ENGINE_PROXY_H
#define C_ENGINE_PROXY_H
#include <cstdint>
#include <Singleton.hpp>

class EngineProxy {
    friend class Singleton<EngineProxy>;
public:
    EngineProxy(const EngineProxy&) = delete;
    EngineProxy& operator=(const EngineProxy&) = delete;
public:
    void init();
    void setViewPort(int32_t width, int32_t height);
    void setBackground(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    void update(int64_t deltaTime);
    void render(int64_t deltaTime);
    void release();
private:
    EngineProxy() = default;
    ~EngineProxy() = default;
private:

};

#define Engine Singleton<EngineProxy>::getInstance()
#endif
