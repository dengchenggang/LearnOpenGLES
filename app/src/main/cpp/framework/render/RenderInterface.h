#ifndef C_RENDER_INTERFACE_H
#define C_RENDER_INTERFACE_H
#include "Singleton.hpp"
#include "IRenderInterface.h"
#include <memory>

class RenderInterfaceProxy {
    friend class Singleton<RenderInterfaceProxy>;
public:
    RenderInterfaceProxy(const RenderInterfaceProxy&) = delete;
    RenderInterfaceProxy& operator = (const RenderInterfaceProxy&) = delete;
public:
    IRenderInterface& getProxy() const noexcept { return *mRenderInterface; }
private:
    RenderInterfaceProxy();
    ~RenderInterfaceProxy() = default;
private:
    std::unique_ptr<IRenderInterface> mRenderInterface;
};

#define RenderInterface Singleton<RenderInterfaceProxy>::getInstance().getProxy()
#endif
