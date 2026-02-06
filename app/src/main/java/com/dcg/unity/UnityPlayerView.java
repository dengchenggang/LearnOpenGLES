package com.dcg.unity;

import android.content.Context;
import android.content.res.Configuration;
import android.util.AttributeSet;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;
import android.view.ViewParent;
import android.widget.FrameLayout;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.lifecycle.DefaultLifecycleObserver;
import androidx.lifecycle.LifecycleOwner;

import com.dcg.utils.Debug;
import com.unity3d.player.UnityPlayer;

public class UnityPlayerView extends FrameLayout implements DefaultLifecycleObserver {

    private static final String TAG = "UnityPlayerView::";

    private final UnityPlayer mUnityPlayer;

    public UnityPlayerView(@NonNull Context context) {
        this(context, null, 0, 0);
    }

    public UnityPlayerView(@NonNull Context context, @Nullable AttributeSet attrs) {
        this(context, attrs, 0, 0);
    }

    public UnityPlayerView(@NonNull Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        this(context, attrs, defStyleAttr, 0);
    }

    public UnityPlayerView(@NonNull Context context, @Nullable AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);
        mUnityPlayer = new UnityPlayer(context);
        ViewParent p = mUnityPlayer.getView().getParent();
        if (p instanceof ViewGroup) {
            ((ViewGroup) p).removeView(mUnityPlayer.getView());
        }
        addView(mUnityPlayer.getView(), new FrameLayout.LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT));
        registerLifecycle(context);
        final SurfaceView surfaceView = UnityPlayerUtils.findSurfaceView(mUnityPlayer);
        if (surfaceView != null) {
            surfaceView.getHolder().addCallback(mSurfaceViewProxy);
        } else {
            Debug.logE(TAG + "UnityPlayerView: surfaceView is null");
        }
    }

    public void setUnityVisible(boolean visible) {
        Debug.logI(TAG + "setUnityVisible: visible=" + visible);
        mUnityPlayer.setVisibility(visible ? View.VISIBLE : View.GONE);
        if (visible) {
            if (mSurfaceViewProxy.getSurfaceCreated()) {
                mUnityPlayer.onResume();
            }
        } else {
            mUnityPlayer.onPause();
        }
    }

    public void setTransparent(boolean on) {
        Debug.logI(TAG + "setTransparent: on=" + on);
        mUnityPlayer.setBackgroundColor(on ? 0x00000000 : 0xFF000000);
    }

    @Override
    public void onCreate(@NonNull LifecycleOwner owner) {
        DefaultLifecycleObserver.super.onCreate(owner);
        Debug.logI(TAG + "onCreate");
    }

    @Override
    public void onStart(@NonNull LifecycleOwner owner) {
        Debug.logI(TAG + "onStart");
        DefaultLifecycleObserver.super.onStart(owner);
        mUnityPlayer.onStart();
    }

    @Override
    public void onResume(@NonNull LifecycleOwner owner) {
        final int visible = mUnityPlayer.getVisibility();
        Debug.logI(TAG + "onResume: surfaceCreated=" + mSurfaceViewProxy.getSurfaceCreated() + ", visible=" + visible);
        DefaultLifecycleObserver.super.onResume(owner);
        if (mSurfaceViewProxy.getSurfaceCreated() && (visible == View.VISIBLE)) {
            mUnityPlayer.onResume();
        }
    }

    @Override
    public void onPause(@NonNull LifecycleOwner owner) {
        Debug.logI(TAG + "onPause");
        DefaultLifecycleObserver.super.onPause(owner);
        mUnityPlayer.onPause();
    }

    @Override
    public void onStop(@NonNull LifecycleOwner owner) {
        Debug.logI(TAG + "onStop");
        DefaultLifecycleObserver.super.onStop(owner);
        mUnityPlayer.onStop();
    }

    @Override
    public void onDestroy(@NonNull LifecycleOwner owner) {
        Debug.logI(TAG + "onDestroy");
        DefaultLifecycleObserver.super.onDestroy(owner);
        mUnityPlayer.destroy();
    }

    @Override
    protected void onWindowVisibilityChanged(int visibility) {
        Debug.logI(TAG + "onWindowVisibilityChanged: visibility=" + visibility);
        super.onWindowVisibilityChanged(visibility);
    }

    @Override
    public void onWindowFocusChanged(boolean hasWindowFocus) {
        Debug.logI(TAG + "onWindowFocusChanged: hasWindowFocus=" + hasWindowFocus);
        super.onWindowFocusChanged(hasWindowFocus);
        mUnityPlayer.windowFocusChanged(hasWindowFocus);
    }

    @Override
    protected void onConfigurationChanged(Configuration newConfig) {
        Debug.logI(TAG + "onConfigurationChanged: newConfig=" + newConfig);
        super.onConfigurationChanged(newConfig);
        mUnityPlayer.configurationChanged(newConfig);
    }

    private void registerLifecycle(Context context) {
        if (context instanceof LifecycleOwner) {
            ((LifecycleOwner) context).getLifecycle().addObserver(this);
        } else {
            Debug.logE(TAG + "registerLifecycle: context isn't LifecycleOwner");
        }
    }

    private class UnityPlayerSurfaceSurfaceHolderCallback implements SurfaceHolder.Callback {

        private boolean mSurfaceCreated = false;

        public boolean getSurfaceCreated() { return mSurfaceCreated; }

        @Override
        public void surfaceCreated(@NonNull SurfaceHolder holder) {
            final int visible = mUnityPlayer.getVisibility();
            Debug.logI(TAG + "surfaceCreated: visible=" + visible);

            if (visible == View.VISIBLE) {
                mUnityPlayer.onResume();
            }
            mSurfaceCreated = true;
        }

        @Override
        public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {
            Debug.logI(TAG + "surfaceChanged: width=" + width + ", height=" + height);
        }

        @Override
        public void surfaceDestroyed(@NonNull SurfaceHolder holder) {
            final int visible = mUnityPlayer.getVisibility();
            Debug.logI(TAG + "surfaceDestroyed: visible=" + visible);
            mSurfaceCreated = false;
            mUnityPlayer.onPause();
        }
    }

    private final UnityPlayerSurfaceSurfaceHolderCallback mSurfaceViewProxy = new UnityPlayerSurfaceSurfaceHolderCallback();
}
