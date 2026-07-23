#include "FileSystem.h"
#include "ViewManager.h"
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <jni.h>
#include <string>

extern "C" {

JNIEXPORT void JNICALL
Java_com_dcg_learnopengles_NativeBridge_nativeInit(JNIEnv* env, jclass , jobject assetManager, jstring filesDir, jint gles) {
    const char* filesDirPath = env->GetStringUTFChars(filesDir, nullptr);
    std::string filesDirStr;
    if (filesDirPath) {
        filesDirStr = filesDirPath;
        env->ReleaseStringUTFChars(filesDir, filesDirPath);
    }
    FileSystem.SetReader(filesDirStr);
    FileSystem.SetReader(AAssetManager_fromJava(env, assetManager));
    FileSystem.SetWriter(filesDirStr);
    ViewManager.init(gles);
}

JNIEXPORT void JNICALL
Java_com_dcg_learnopengles_NativeBridge_nativeBind(JNIEnv* env, jclass, jobject surface) {
    ANativeWindow* window = ANativeWindow_fromSurface(env, surface);
    ViewManager.bind(window);
}

JNIEXPORT void JNICALL
Java_com_dcg_learnopengles_NativeBridge_nativeResize(JNIEnv* env, jclass, jint w, jint h) {
    ViewManager.resize(w, h);
}

JNIEXPORT void JNICALL
Java_com_dcg_learnopengles_NativeBridge_nativeUnbind(JNIEnv* env, jclass) {
    ViewManager.unbind();
}

JNIEXPORT void JNICALL
Java_com_dcg_learnopengles_NativeBridge_nativeDestroy(JNIEnv* env, jclass) {
    ViewManager.destroy();
    FileSystem.reset();
}
}