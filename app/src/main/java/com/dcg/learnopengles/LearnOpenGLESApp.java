package com.dcg.learnopengles;

import android.app.Application;

public class LearnOpenGLESApp extends Application {

    @Override
    public void onCreate() {
        super.onCreate();
        // 初始化（传入 AssetManager、FilesDir 绝对路径和 GLES 版本）
        NativeBridge.nativeInit(getAssets(), getFilesDir().getAbsolutePath(), 3);
    }
}
