#ifndef I_RENDERER_H
#define I_RENDERER_H
#include <cstdint>

class IRenderer {

public:
    IRenderer() = default;
    virtual ~IRenderer() = default;
    IRenderer(const IRenderer&) = delete;
    IRenderer& operator= (const IRenderer&) = delete;
public:
    virtual void initialize(int32_t width, int32_t height) = 0;
    virtual void resize(int32_t width, int32_t height) = 0;
    virtual void update(int64_t deltaTime) = 0;
    virtual void render(int64_t deltaTime) = 0;
    virtual void destroy() = 0;
};

#endif
