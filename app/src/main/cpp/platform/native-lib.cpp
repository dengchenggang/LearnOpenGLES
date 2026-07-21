#include "CEGLSurface.h"
#include "AssetManagerReader.h"
#include "FileSystem.h"
#include "Log.h"
#include "OpenGLESRenderInterface.h"
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <jni.h>
#include <string>
#include <memory>

class EGLSurfaceManager {
private:
    static constexpr const char* TAG {"EGLSurfaceManager"};

    CEGLSurface mEGLSurface {};
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
Java_com_dcg_learnopengles_NativeBridge_nativeInit(JNIEnv* env, jclass , jobject assetManager, jstring filesDir, jint gles) {
    // 初始化文件系统（Assets + FilesDir）
    FileSystem.SetReader(AAssetManager_fromJava(env, assetManager));

    const char* filesDirPath = env->GetStringUTFChars(filesDir, nullptr);
    if (filesDirPath) {
        FileSystem.SetReader(filesDirPath);
        env->ReleaseStringUTFChars(filesDir, filesDirPath);
    }

    // 初始化 EGL
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