#include "CEGLDisplay.h"
#include <EGL/egl.h>

CEGLDisplay::CEGLDisplay() : mDisplay(EGL_NO_DISPLAY)
{
    initialize();
}

CEGLDisplay::~CEGLDisplay() {
    if (valid()) {
        eglTerminate(mDisplay);
    }
}

void CEGLDisplay::initialize() {
    mDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (valid()) {
        const EGLint cfgAttr[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE, 8,
            EGL_ALPHA_SIZE, 8, EGL_DEPTH_SIZE, 16,
            EGL_NONE
        };
        EGLint numCfg {};
        eglChooseConfig(mDisplay, cfgAttr, &mConfig, 1, &numCfg);
    }
}

bool CEGLDisplay::valid() const{ return mDisplay != EGL_NO_DISPLAY; }