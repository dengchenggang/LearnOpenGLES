#include "OpenGLESRenderContext.h"
#include "utils/Log.h"
#include <GLES3/gl3.h>
#include <limits>

KEY_VALUE(TAG, OpenGLESRenderContext)

namespace engine {
namespace renderer {

OpenGLESRenderContext::OpenGLESRenderContext()
    : mDisplay(EGL_NO_DISPLAY)
    , mConfig(nullptr)
    , mContext(EGL_NO_CONTEXT)
    , mSurface(EGL_NO_SURFACE)
    , mPbufferSurface(EGL_NO_SURFACE) {
}

OpenGLESRenderContext::~OpenGLESRenderContext() {
    deInitialize();
}

bool OpenGLESRenderContext::initialize(std::int32_t gles) {
    LogI("%s initialize enter.", TAG);

    if (mInitialized) {
        LogW("%s already initialized!", TAG);
        return true;
    }

    mGLESVersion = gles;

    mDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (mDisplay == EGL_NO_DISPLAY) {
        LogE("eglGetDisplay failed");
        return false;
    }
    eglInitialize(mDisplay, nullptr, nullptr);
    const EGLint cfgAttr[] = {
            EGL_RENDERABLE_TYPE, gles >= 3 ? EGL_OPENGL_ES3_BIT : EGL_OPENGL_ES2_BIT,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT | EGL_PBUFFER_BIT,
            EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE, 8,
            EGL_ALPHA_SIZE, 8, EGL_DEPTH_SIZE, 16,
            EGL_NONE
    };
    EGLint numCfg {std::numeric_limits<EGLint>::min()};
    eglChooseConfig(mDisplay, cfgAttr, &mConfig, 1, &numCfg);

    const EGLint ctxAttr[] = {EGL_CONTEXT_CLIENT_VERSION, gles, EGL_NONE};
    mContext = eglCreateContext(mDisplay, mConfig, nullptr, ctxAttr);
    if (mContext == EGL_NO_CONTEXT) {
        LogE("eglCreateContext failed!");
        return false;
    }

    mInitialized = true;
    LogI("%s EGL initialized successfully.", TAG);
    return true;
}

bool OpenGLESRenderContext::attachOffscreenSurface() {
    LogI("%s attachOffscreenSurface enter.", TAG);

    if (!mInitialized) {
        LogE("%s not initialized!", TAG);
        return false;
    }

    const EGLint pbufferAttr[] = {EGL_WIDTH, 1, EGL_HEIGHT, 1, EGL_NONE};
    mPbufferSurface = eglCreatePbufferSurface(mDisplay, mConfig, pbufferAttr);
    if (mPbufferSurface == EGL_NO_SURFACE) {
        LogE("%s eglCreatePbufferSurface failed!", TAG);
        return false;
    }

    if (!eglMakeCurrent(mDisplay, mPbufferSurface, mPbufferSurface, mContext)) {
        LogE("%s eglMakeCurrent(pbuffer) failed!", TAG);
        eglDestroySurface(mDisplay, mPbufferSurface);
        mPbufferSurface = EGL_NO_SURFACE;
        return false;
    }

    mBound = true;
    LogI("%s attachOffscreenSurface success, pbuffer context active.", TAG);
    return true;
}

bool OpenGLESRenderContext::attachSurface(ANativeWindow *window) {
    LogI("%s attachSurface enter.", TAG);

    if (!mInitialized) {
        LogE("%s not initialized!", TAG);
        return false;
    }

    if (mSurface != EGL_NO_SURFACE) {
        LogE("mSurface already exists!");
        return false;
    }

    if (mPbufferSurface != EGL_NO_SURFACE) {
        eglMakeCurrent(mDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglDestroySurface(mDisplay, mPbufferSurface);
        mPbufferSurface = EGL_NO_SURFACE;
        mBound = false;
    }

    mSurface = eglCreateWindowSurface(mDisplay, mConfig, window, nullptr);

    if (mSurface == EGL_NO_SURFACE) {
        LogE("eglCreateWindowSurface failed!");
        return false;
    }

    if (!eglMakeCurrent(mDisplay, mSurface, mSurface, mContext)) {
        LogE("eglMakeCurrent failed!");
        return false;
    }

    eglQuerySurface(mDisplay, mSurface, EGL_WIDTH, &mWidth);
    eglQuerySurface(mDisplay, mSurface, EGL_HEIGHT, &mHeight);

    mBound = true;

    LogI("%s attachSurface success: width=%d, height=%d.", TAG, mWidth, mHeight);
    return true;
}

void OpenGLESRenderContext::setViewPort(std::int32_t x, std::int32_t y, std::int32_t w, std::int32_t h) {
    LogI("%s setViewPort, x=%d, y=%d, width=%d, height=%d", TAG, x, y, w, h);
    if (!mBound) {
        return;
    }
    glViewport(x, y, w, h);
}

void OpenGLESRenderContext::swapBuffers() {
    eglSwapBuffers(mDisplay, mSurface);
}

void OpenGLESRenderContext::detachSurface() {
    LogI("%s detachSurface enter", TAG);

    eglMakeCurrent(mDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if (mSurface != EGL_NO_SURFACE) {
        eglDestroySurface(mDisplay, mSurface);
        mSurface = EGL_NO_SURFACE;
    }
    if (mPbufferSurface != EGL_NO_SURFACE) {
        eglDestroySurface(mDisplay, mPbufferSurface);
        mPbufferSurface = EGL_NO_SURFACE;
    }
    mBound = false;

    LogI("%s detachSurface done", TAG);
}

void OpenGLESRenderContext::deInitialize() {
    LogI("%s deInitialize enter.", TAG);

    if (mDisplay != EGL_NO_DISPLAY) {
        eglMakeCurrent(mDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (mSurface != EGL_NO_SURFACE) {
            eglDestroySurface(mDisplay, mSurface);
            mSurface = EGL_NO_SURFACE;
        }
        if (mPbufferSurface != EGL_NO_SURFACE) {
            eglDestroySurface(mDisplay, mPbufferSurface);
            mPbufferSurface = EGL_NO_SURFACE;
        }
        if (mContext != EGL_NO_CONTEXT) {
            eglDestroyContext(mDisplay, mContext);
            mContext = EGL_NO_CONTEXT;
        }
        eglTerminate(mDisplay);
        mDisplay = EGL_NO_DISPLAY;
    }

    mInitialized = false;
    mBound = false;

    LogI("%s deInitialize exit.", TAG);
}

} // namespace renderer
} // namespace engine
