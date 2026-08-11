#ifndef C_RENDER_INTERFACE_H
#define C_RENDER_INTERFACE_H
#include "Singleton.hpp"
#include "IRenderInterface.h"
#include <memory>

namespace engine {
namespace renderer {

class RenderInterface {
    friend class Singleton<RenderInterface>;
public:
    RenderInterface(const RenderInterface&) = delete;
    RenderInterface& operator = (const RenderInterface&) = delete;
public:
    IRenderInterface& getProxy() const noexcept { return *mRenderInterface; }
private:
    RenderInterface();
    ~RenderInterface() = default;
private:
    std::unique_ptr<IRenderInterface> mRenderInterface;
};

} // namespace renderer
} // namespace engine

#define RenderInterface Singleton<engine::renderer::RenderInterface>::getInstance().getProxy()
#endif
