#ifndef C_ENGINE_H
#define C_ENGINE_H
#include <cstdint>
#include <Singleton.hpp>

class Engine {
    friend class Singleton<Engine>;
public:
    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;
public:
    void init();
    void setBackground(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    void start();
    void stop();
    void release();
private:
    Engine() = default;
    ~Engine() = default;
};

#define ENGINE Singleton<Engine>::getInstance()
#endif
