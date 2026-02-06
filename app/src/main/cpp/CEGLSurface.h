//
// Created by banma-3361 on 2025/11/4.
//

#ifndef LEARNOPENGLES_CEGLSURFACE_H
#define LEARNOPENGLES_CEGLSURFACE_H
#include <android/native_window_jni.h>
#include <cstdint>
#include <memory>

class IRender;


class CEGLSurface {
private:
    void* mDisplay;
    void* mSurface;
    void* mContext;
    int mWidth;
    int mHeight;
    std::unique_ptr<IRender> mRender;
public:
    CEGLSurface(ANativeWindow *window, std::unique_ptr<IRender>&& render);
    CEGLSurface(const CEGLSurface&) = delete;
    CEGLSurface& operator =(const CEGLSurface&) = delete;
    ~CEGLSurface();
public:
    explicit operator bool() const { return !invalid(); }
    void resize(std::int32_t w, std::int32_t h);
    void update(std::int64_t deltaTime);
    void render(std::int64_t deltaTime);
private:
    [[nodiscard]] bool invalid() const;
    bool initialize(ANativeWindow *window, std::int32_t gles);
    void release();
};


#endif //LEARNOPENGLES_CEGLSURFACE_H
