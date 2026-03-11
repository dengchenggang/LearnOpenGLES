package com.dcg.learnopengles;

import android.content.Context;
import android.util.AttributeSet;
import android.view.Choreographer;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import androidx.annotation.NonNull;

public class EGLSurfaceView extends SurfaceView {

    private final Choreographer mChoreographer = Choreographer.getInstance();
    private final Choreographer.FrameCallback mChoreographerFrameCallback = new Choreographer.FrameCallback() {
        @Override
        public void doFrame(long frameTimeNanos) {
            NativeBridge.nativeUpdate(frameTimeNanos);
            NativeBridge.nativeRender(frameTimeNanos);
            mChoreographer.postFrameCallback(mChoreographerFrameCallback);
        }
    };

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
        SurfaceHolder.Callback mSurfaceHolderCallback = new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(@NonNull SurfaceHolder holder) {
                NativeBridge.nativeBind(holder.getSurface());
                mChoreographer.postFrameCallback(mChoreographerFrameCallback);
            }

            @Override
            public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {
                NativeBridge.nativeResize(width, height);
            }

            @Override
            public void surfaceDestroyed(@NonNull SurfaceHolder holder) {
                mChoreographer.removeFrameCallback(mChoreographerFrameCallback);
                NativeBridge.nativeUnbind();
            }
        };
        this.getHolder().addCallback(mSurfaceHolderCallback);
    }
}
