#include "filesystem/FileSystem.h"
#include "Engine.h"
#include "common/ModuleDef.h"
#include "nvs/NVSModule.h"
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

    auto config = std::make_shared<JsonConfigLoader>();
    config->parse(FileSystem.readString("config/learnopengles.json"));

    std::map<std::string, engine::LevelPtr> levels;
    levels.emplace(module::MODULE_NAME_NVS, std::make_unique<module::NVSModule>(Engine, config));
    Engine.init(gles, std::move(levels), module::MODULE_NAME_NVS);
}

JNIEXPORT void JNICALL
Java_com_dcg_learnopengles_NativeBridge_nativeBind(JNIEnv* env, jclass, jobject surface) {
    ANativeWindow* window = ANativeWindow_fromSurface(env, surface);
    Engine.beginPlay(window);
}

JNIEXPORT void JNICALL
Java_com_dcg_learnopengles_NativeBridge_nativeResize(JNIEnv* env, jclass, jint w, jint h) {
    Engine.resize(w, h);
}

JNIEXPORT void JNICALL
Java_com_dcg_learnopengles_NativeBridge_nativeUnbind(JNIEnv* env, jclass) {
    Engine.endPlay();
}

JNIEXPORT void JNICALL
Java_com_dcg_learnopengles_NativeBridge_nativeDestroy(JNIEnv* env, jclass) {
    Engine.deInit();
    FileSystem.reset();
}
}