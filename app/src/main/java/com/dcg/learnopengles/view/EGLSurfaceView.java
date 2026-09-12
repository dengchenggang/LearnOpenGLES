package com.dcg.learnopengles.view;

import android.content.Context;
import android.graphics.PixelFormat;
import android.util.AttributeSet;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import androidx.annotation.NonNull;

import com.dcg.learnopengles.NativeBridge;

public class EGLSurfaceView extends SurfaceView {

    public EGLSurfaceView(Context context) {
        this(context, null, 0, 0);
    }

    public EGLSurfaceView(Context context, AttributeSet attrs) {
        this(context, attrs, 0, 0);
    }

    public EGLSurfaceView(Context context, AttributeSet attrs, int defStyleAttr) {
        this(context, attrs, defStyleAttr, 0);
    }

    public EGLSurfaceView(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);
        getHolder().setFormat(PixelFormat.TRANSLUCENT);
        getHolder().addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(@NonNull SurfaceHolder holder) {
                NativeBridge.nativeBind(holder.getSurface());
            }

            @Override
            public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {
                NativeBridge.nativeResize(width, height);
            }

            @Override
            public void surfaceDestroyed(@NonNull SurfaceHolder holder) {
                NativeBridge.nativeUnbind();
            }
        });
    }
}
