#ifndef OPENGLES_RENDERER_H
#define OPENGLES_RENDERER_H
#include "IRenderer.h"

class OpenGLESRenderer : public IRenderer {
public:
    OpenGLESRenderer() = default;
    ~OpenGLESRenderer() override = default;
    OpenGLESRenderer(const OpenGLESRenderer&) = delete;
    OpenGLESRenderer& operator = (const OpenGLESRenderer&) = delete;
public:
    void initialize(int32_t width, int32_t height) override;
    void resize(int32_t width, int32_t height) override;
    void update(int64_t deltaTime) override;
    void render(int64_t deltaTime) override;
    void destroy() override;
private:
    int32_t mWidth {0};
    int32_t mHeight {0};
};
#endif
