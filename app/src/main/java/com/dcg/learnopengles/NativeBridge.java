package com.dcg.learnopengles;

import android.view.Surface;
import android.view.SurfaceView;

public class NativeBridge {
    static { System.loadLibrary("learnopengles");}

    public static native void nativeInit(int gles);
    public static native void nativeBind(Surface surface);
    public static native void nativeResize(int w, int h);
    public static native void nativeUpdate(long deltaTime);
    public static native void nativeRender(long deltaTime);

    public static native void nativeUnbind();
    public static native void nativeDestroy();
}
