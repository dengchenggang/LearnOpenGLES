//
// Created by banma-3361 on 2025/11/4.
//

#include "CEGLSurface.h"
#include "CLogUtils.h"
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <limits>
#include <android/log.h>
#include "IRender.h"

KEY_VALUE(TAG, CEGLSurface)

CEGLSurface::CEGLSurface(ANativeWindow *window, std::unique_ptr<IRender>&& render)
    : mDisplay(EGL_NO_DISPLAY), mContext(EGL_NO_CONTEXT), mSurface(EGL_NO_SURFACE), mWidth(0), mHeight(0), mRender(std::move(render))
{
    this->initialize(window, 3);
}

CEGLSurface::~CEGLSurface() {
    this->release();
}

bool CEGLSurface::invalid() const {
    return (mDisplay == EGL_NO_DISPLAY) || (mContext == EGL_NO_CONTEXT) || (mSurface == EGL_NO_SURFACE);
}

void CEGLSurface::resize(std::int32_t w, std::int32_t h) {
    LogI("%s resize: %d -> %d, %d -> %d", TAG, mWidth, w, mHeight, h);
    mWidth = w;
    mHeight = h;
    glViewport(0, 0, w, h);
    if (mRender) {
        mRender->resize(w, h);
    }
}

void CEGLSurface::update(std::int64_t deltaTime) {
    if (mRender) {
        mRender->update(deltaTime);
    }
}

void CEGLSurface::render(std::int64_t deltaTime) {
    glViewport(0, 0, mWidth, mHeight);
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    if (mRender) {
        mRender->render(deltaTime);
    }
    eglSwapBuffers(mDisplay, mSurface);
}

bool CEGLSurface::initialize(ANativeWindow *window, std::int32_t gles) {
    mDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (mDisplay == EGL_NO_DISPLAY) {
        LogUtils.e("eglGetDisplay failed");
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
    EGLConfig config {nullptr};
    EGLint numCfg {std::numeric_limits<EGLint>::min()};
    eglChooseConfig(mDisplay, cfgAttr, &config, 1, &numCfg);

    const EGLint ctxAttr[] = {EGL_CONTEXT_CLIENT_VERSION, gles, EGL_NONE};
    mContext = eglCreateContext(mDisplay, config, nullptr, ctxAttr);
    if (mContext == EGL_NO_CONTEXT) {
        LogUtils.e("eglCreateContext failed!");
        return false;
    }

    mSurface = eglCreateWindowSurface(mDisplay, config, window, nullptr);
    if (mSurface == EGL_NO_SURFACE) {
        LogUtils.e("eglCreateWindowSurface failed!");
        return false;
    }

    if (!eglMakeCurrent(mDisplay, mSurface, mSurface, mContext)) {
        LogUtils.e("eglMakeCurrent failed!");
        return false;
    }

    eglQuerySurface(mDisplay, mSurface, EGL_WIDTH, &mWidth);
    eglQuerySurface(mDisplay, mSurface, EGL_HEIGHT, &mHeight);
    LogI("%s initialize: mWidth=%d, mHeight=%d", TAG, mWidth, mHeight);
    if (mRender) {
        mRender->initialize(mWidth, mHeight);
    }
    return true;
}

void CEGLSurface::release() {
    if (mRender) {
        mRender->destroy();
    }
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
}