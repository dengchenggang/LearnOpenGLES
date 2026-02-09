#ifndef I_RENDERER_H
#define I_RENDERER_H

class IRenderer {

public:
    IRenderer() = default;
    virtual ~IRenderer() = default;
    IRenderer(const IRenderer&) = delete;
    IRenderer& operator= (const IRenderer&) = delete;
public:
    virtual void resize(int width, int height) = 0;
    
};

#endif
