package com.dcg.learnopengles;

import android.app.Application;
import android.content.Intent;

import com.dcg.learnopengles.service.RenderService;

public class LearnOpenGLESApp extends Application {

    @Override
    public void onCreate() {
        super.onCreate();
        NativeBridge.nativeInit(getAssets(), getFilesDir().getAbsolutePath(), 3);
        startForegroundService(new Intent(this, RenderService.class));
    }
}
