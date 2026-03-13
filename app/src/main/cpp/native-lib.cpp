#include "CEGLSurface.h"
#include "LogUtils.h"
#include "OpenGLESRenderInterface.h"
#include <jni.h>
#include <string>
#include <memory>

class EGLSurfaceManager {
private:
    static constexpr const char* TAG {"EGLSurfaceManager"};

    CEGLSurface mEGLSurface {std::make_unique<OpenGLESRenderInterface>()};
private:
    EGLSurfaceManager() = default;
    ~EGLSurfaceManager() = default;
public:
    EGLSurfaceManager(const EGLSurfaceManager&) = delete;
    EGLSurfaceManager& operator= (const EGLSurfaceManager&) = delete;

    static EGLSurfaceManager& getInstance();
public:
    void init(std::int32_t gles);
    void bind(ANativeWindow *window);
    void resize(std::int32_t w, std::int32_t h);
    void unbind();
    void destroy();
};

EGLSurfaceManager& EGLSurfaceManager::getInstance() {
    static EGLSurfaceManager sEGLSurfaceManager {};
    return sEGLSurfaceManager;
}

void EGLSurfaceManager::init(std::int32_t gles) {
    mEGLSurface.initialize(gles);
}

void EGLSurfaceManager::bind(ANativeWindow *window) {
    mEGLSurface.bind(window);
}

void EGLSurfaceManager::resize(std::int32_t w, std::int32_t h) {
    mEGLSurface.resize(w, h);
}

void EGLSurfaceManager::unbind() {
    mEGLSurface.unbind();
}

void EGLSurfaceManager::destroy() {
    mEGLSurface.release();
}

extern "C" {
JNIEXPORT void JNICALL
Java_com_dcg_learnopengles_NativeBridge_nativeInit(JNIEnv* env, jclass , jint gles) {
    EGLSurfaceManager::getInstance().init(gles);
}

JNIEXPORT void JNICALL
Java_com_dcg_learnopengles_NativeBridge_nativeBind(JNIEnv* env, jclass, jobject surface) {
    ANativeWindow* window = ANativeWindow_fromSurface(env, surface);
    EGLSurfaceManager::getInstance().bind(window);
}

JNIEXPORT void JNICALL
Java_com_dcg_learnopengles_NativeBridge_nativeResize(JNIEnv* env, jclass, jint w, jint h) {
    EGLSurfaceManager::getInstance().resize(w, h);
}

JNIEXPORT void JNICALL
Java_com_dcg_learnopengles_NativeBridge_nativeUnbind(JNIEnv* env, jclass) {
    EGLSurfaceManager::getInstance().unbind();
}

JNIEXPORT void JNICALL
Java_com_dcg_learnopengles_NativeBridge_nativeDestroy(JNIEnv* env, jclass) {
    EGLSurfaceManager::getInstance().destroy();
}
}