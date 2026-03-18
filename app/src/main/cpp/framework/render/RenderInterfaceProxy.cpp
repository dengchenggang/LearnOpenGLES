#include "RenderInterface.h"
#include "OpenGLESRenderInterface.h"

RenderInterfaceProxy::RenderInterfaceProxy()
    : mRenderInterface {std::make_unique<OpenGLESRenderInterface>()} {

}