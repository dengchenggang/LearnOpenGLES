#include "CEGLSurface.h"
#include "CLogUtils.h"
#include <jni.h>
#include <string>
#include <memory>
#include <IRender.h>

class EGLSurfaceManager {
private:
    static constexpr const char* TAG {"EGLSurfaceManager"};

    std::shared_ptr<CEGLSurface> mEGLSurface {nullptr};
private:
    EGLSurfaceManager() = default;
    ~EGLSurfaceManager() = default;
public:
    EGLSurfaceManager(const EGLSurfaceManager&) = delete;
    EGLSurfaceManager& operator= (const EGLSurfaceManager&) = delete;

    static EGLSurfaceManager& getInstance();
public:
    void init(ANativeWindow *window, std::int32_t gles);
    void resize(std::int32_t w, std::int32_t h);
    void update(std::int64_t deltaTime);
    void render(std::int64_t deltaTime);
    void destroy();
};

EGLSurfaceManager& EGLSurfaceManager::getInstance() {
    static EGLSurfaceManager sEGLSurfaceManager {};
    return sEGLSurfaceManager;
}

void EGLSurfaceManager::init(ANativeWindow *window, std::int32_t gles) {
    if (!mEGLSurface) {
        mEGLSurface = std::make_unique<CEGLSurface>(window, nullptr);
    } else {
        LogUtils.i("%s init: mEGLSurface has initialized", TAG);
    }
}

void EGLSurfaceManager::resize(std::int32_t w, std::int32_t h) {
    if (mEGLSurface) {
        mEGLSurface->resize(w, h);
    }
}

void EGLSurfaceManager::update(std::int64_t deltaTime) {
    if (mEGLSurface) {
        mEGLSurface->update(deltaTime);
    }
}

void EGLSurfaceManager::render(std::int64_t deltaTime) {
    if (mEGLSurface) {
        mEGLSurface->render(deltaTime);
    }
}

void EGLSurfaceManager::destroy() {
    if (mEGLSurface) {
        mEGLSurface.reset();
    }
}

extern "C" {
JNIEXPORT void JNICALL
Java_com_dcg_learnopengles_NativeBridge_nativeInit(JNIEnv* env, jclass , jobject surface, jint gles) {
    ANativeWindow* window = ANativeWindow_fromSurface(env, surface);
    EGLSurfaceManager::getInstance().init(window, gles);
}

JNIEXPORT void JNICALL
Java_com_dcg_learnopengles_NativeBridge_nativeResize(JNIEnv* env, jclass, jint w, jint h) {
    EGLSurfaceManager::getInstance().resize(w, h);
}

JNIEXPORT void JNICALL
Java_com_dcg_learnopengles_NativeBridge_nativeUpdate(JNIEnv* env, jclass, jlong deltaTime) {
    EGLSurfaceManager::getInstance().update(deltaTime);
}

JNIEXPORT void JNICALL
Java_com_dcg_learnopengles_NativeBridge_nativeRender(JNIEnv* env, jclass, jlong deltaTime) {
    EGLSurfaceManager::getInstance().render(deltaTime);
}

JNIEXPORT void JNICALL
Java_com_dcg_learnopengles_NativeBridge_nativeDestroy(JNIEnv* env, jclass) {
    EGLSurfaceManager::getInstance().destroy();
}
}