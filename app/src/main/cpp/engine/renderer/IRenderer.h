#ifndef I_RENDERER_H
#define I_RENDERER_H
#include <cstdint>

namespace engine {


class IRenderer {

public:
    IRenderer() = default;
    virtual ~IRenderer() = default;
    IRenderer(const IRenderer&) = delete;
    IRenderer& operator= (const IRenderer&) = delete;
public:
    virtual void initialize() = 0;
    virtual void update(int64_t deltaTime) = 0;
    virtual void render(int64_t deltaTime) = 0;
    virtual void release() = 0;
};


} // namespace engine

#endif