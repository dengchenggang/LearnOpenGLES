//
// Created by banma-3361 on 2025/11/4.
//

#include "CEGLSurface.h"
#include "LogUtils.h"
#include <GLES3/gl3.h>
#include <limits>
#include <android/log.h>
#include "IRenderer.h"

KEY_VALUE(TAG, CEGLSurface)

bool CEGLSurface::initialize(std::int32_t gles) {
    mDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (mDisplay == EGL_NO_DISPLAY) {
        Log.e("eglGetDisplay failed");
        return false;
    }
    eglInitialize(mDisplay, nullptr, nullptr);
    const EGLint cfgAttr[] = {
            EGL_RENDERABLE_TYPE, gles >= 3 ? EGL_OPENGL_ES3_BIT : EGL_OPENGL_ES2_BIT,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE, 8,
            EGL_ALPHA_SIZE, 8, EGL_DEPTH_SIZE, 16,
            EGL_NONE
    };
    EGLint numCfg {std::numeric_limits<EGLint>::min()};
    eglChooseConfig(mDisplay, cfgAttr, &mConfig, 1, &numCfg);

    const EGLint ctxAttr[] = {EGL_CONTEXT_CLIENT_VERSION, gles, EGL_NONE};
    mContext = eglCreateContext(mDisplay, mConfig, nullptr, ctxAttr);
    if (mContext == EGL_NO_CONTEXT) {
        Log.e("eglCreateContext failed!");
        return false;
    }

    return true;
}

bool CEGLSurface::bind(ANativeWindow *window) {
    LogI("%s bind enter.", TAG);

    if (mSurface != EGL_NO_SURFACE) {
        LogE("mSurface already exists!");
        return false;
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

    GLint width {0}, height {0};
    eglQuerySurface(mDisplay, mSurface, EGL_WIDTH, &width);
    eglQuerySurface(mDisplay, mSurface, EGL_HEIGHT, &height);

    mRender->initialize(width, width);
    LogI("%s bind exit: mWidth=%d, mHeight=%d.", TAG, width, height);
    return true;
}

void CEGLSurface::resize(std::int32_t w, std::int32_t h) {
    LogI("%s resize, width=%d, height=%d", TAG, w, h);
    mRender->resize(w, h);
}

void CEGLSurface::update(std::int64_t deltaTime) {
    LogI("%s update, deltaTime=%ld", TAG, deltaTime);
    mRender->update(deltaTime);
}

void CEGLSurface::render(std::int64_t deltaTime) {
    mRender->render(deltaTime);
    eglSwapBuffers(mDisplay, mSurface);
}

void CEGLSurface::unbind() {
    LogI("%s unbind enter", TAG);
    eglMakeCurrent(mDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if (mSurface != EGL_NO_SURFACE) {
        eglDestroySurface(mDisplay, mSurface);
        mSurface = EGL_NO_SURFACE;
    }
    LogI("%s unbind exit", TAG);
}

void CEGLSurface::release() {
    LogI("%s release enter.", TAG);
    mRender->destroy();
    if (mDisplay != EGL_NO_DISPLAY) {
        eglMakeCurrent(mDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (mSurface != EGL_NO_SURFACE) {
            eglDestroySurface(mDisplay, mSurface);
            mSurface = EGL_NO_SURFACE;
        }
        if (mContext != EGL_NO_CONTEXT) {
            eglDestroyContext(mDisplay, mContext);
            mContext = EGL_NO_CONTEXT;
        }
        eglTerminate(mDisplay);
        mDisplay = EGL_NO_DISPLAY;
    }
    LogI("%s release exit.", TAG);
}
