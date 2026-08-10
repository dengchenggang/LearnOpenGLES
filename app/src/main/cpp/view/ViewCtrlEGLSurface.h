#ifndef LEARNOPENGLES_VIEWCTRLEGLSURFACE_H
#define LEARNOPENGLES_VIEWCTRLEGLSURFACE_H

#include <EGL/egl.h>
#include <android/native_window_jni.h>
#include <cstdint>

namespace view {

class ViewCtrlEGLSurface {
public:
    ViewCtrlEGLSurface();
    ~ViewCtrlEGLSurface();

    ViewCtrlEGLSurface(const ViewCtrlEGLSurface&) = delete;
    ViewCtrlEGLSurface& operator =(const ViewCtrlEGLSurface&) = delete;

public:
    // EGL 初始化与释放
    bool initialize(std::int32_t gles);
    bool bind(ANativeWindow *window);
    void unbind();
    void release();

    // 交换缓冲区
    void swapBuffers();

    // 设置 OpenGL viewport
    void setViewPort(std::int32_t w, std::int32_t h);

    // 获取 surface 尺寸
    std::int32_t getWidth() const { return mWidth; }
    std::int32_t getHeight() const { return mHeight; }

    // 检查状态
    bool isInitialized() const { return mInitialized; }
    bool isBound() const { return mBound; }

private:
    EGLDisplay mDisplay;
    EGLConfig mConfig;
    EGLContext mContext;
    EGLSurface mSurface;

    std::int32_t mWidth{0};
    std::int32_t mHeight{0};

    bool mInitialized{false};
    bool mBound{false};
    std::int32_t mGLESVersion{3};
};

}  // namespace view

#endif
