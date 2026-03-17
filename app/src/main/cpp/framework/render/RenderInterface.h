#ifndef C_RENDER_INTERFACE_H
#define C_RENDER_INTERFACE_H
#include "Singleton.hpp"
#include "OpenGLESRenderInterface.h"

#define RenderInterface Singleton<OpenGLESRenderInterface>::getInstance()
#endif
