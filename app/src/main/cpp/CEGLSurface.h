//
// Created by banma-3361 on 2025/11/4.
//

#ifndef LEARNOPENGLES_CEGLSURFACE_H
#define LEARNOPENGLES_CEGLSURFACE_H
#include <EGL/egl.h>
#include <android/native_window_jni.h>
#include <cstdint>
#include <memory>
#include "IRenderer.h"


class CEGLSurface {
private:
    std::unique_ptr<IRenderer> mRender;
    EGLDisplay mDisplay;
    EGLConfig mConfig;
    EGLContext mContext;
    EGLSurface mSurface;
public:
    explicit CEGLSurface(std::unique_ptr<IRenderer>&& render) : mRender(std::move(render)), mDisplay(EGL_NO_DISPLAY), mConfig(nullptr), mContext(EGL_NO_CONTEXT), mSurface(EGL_NO_SURFACE) { }
    ~CEGLSurface() { this->release(); }
    CEGLSurface(const CEGLSurface&) = delete;
    CEGLSurface& operator =(const CEGLSurface&) = delete;
public:
    bool initialize(std::int32_t gles);
    bool bind(ANativeWindow *window);
    void resize(std::int32_t w, std::int32_t h);
    void update(std::int64_t deltaTime);
    void render(std::int64_t deltaTime);
    void release();
    void unbind();
};


#endif //LEARNOPENGLES_CEGLSURFACE_H
