#include "RenderInterface.h"
#include "OpenGLESRenderInterface.h"

#undef RenderInterface

namespace engine {
namespace renderer {

RenderInterface::RenderInterface()
    : mRenderInterface {std::make_unique<OpenGLESRenderInterface>()} {

}

} // namespace renderer
} // namespace engine
