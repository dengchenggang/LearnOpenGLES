package com.dcg.learnopengles;

import android.content.res.AssetManager;
import android.view.Surface;
import android.view.SurfaceView;

public class NativeBridge {
    static { System.loadLibrary("learnopengles");}

    // 初始化（传入 AssetManager、FilesDir 绝对路径和 GLES 版本）
    public static native void nativeInit(AssetManager assetManager, String filesDir, int gles);

    // EGL 相关
    public static native void nativeBind(Surface surface);
    public static native void nativeResize(int w, int h);
    public static native void nativeUnbind();
    public static native void nativeDestroy();
}
