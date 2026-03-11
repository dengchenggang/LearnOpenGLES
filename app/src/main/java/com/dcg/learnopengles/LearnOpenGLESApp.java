package com.dcg.learnopengles;

import android.app.Application;

public class LearnOpenGLESApp extends Application {

    @Override
    public void onCreate() {
        super.onCreate();
        NativeBridge.nativeInit(3);
    }
}
