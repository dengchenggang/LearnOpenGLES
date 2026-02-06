package com.dcg.unity;

import androidx.activity.EdgeToEdge;
import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowInsetsCompat;

import com.dcg.utils.Debug;
import com.unity3d.player.IUnityPlayerLifecycleEvents;
import com.unity3d.player.UnityPlayer;

import android.content.Intent;
import android.content.res.Configuration;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class GameActivity extends AppCompatActivity {

    private static final String TAG = "GameActivity::";

    protected UnityPlayer mUnityPlayer;

    public GameActivity() {}

    protected void onCreate(Bundle savedInstanceState) {
        this.requestWindowFeature(1);
        super.onCreate(savedInstanceState);
        EdgeToEdge.enable(this);
        String cmdLine = this.updateUnityCommandLineArguments(this.getIntent().getStringExtra("unity"));
        this.getIntent().putExtra("unity", cmdLine);
        mUnityPlayer = new UnityPlayer(this, mIUnityPlayerLifecycleEvents);
        this.setContentView(mUnityPlayer);
        ViewCompat.setOnApplyWindowInsetsListener(mUnityPlayer, (v, insets) -> {
            Insets systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars());
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom);
            return insets;
        });
        mUnityPlayer.requestFocus();

        final SurfaceView surfaceView = UnityPlayerUtils.findSurfaceView(mUnityPlayer);
        if (surfaceView != null) {
            surfaceView.getHolder().addCallback(new SurfaceHolder.Callback() {
                @Override
                public void surfaceCreated(@NonNull SurfaceHolder holder) {
                    Debug.logI(TAG + "surfaceCreated");
                }

                @Override
                public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {
                    Debug.logI(TAG + "surfaceChanged: width=" + width + ", height=" + height);

                }

                @Override
                public void surfaceDestroyed(@NonNull SurfaceHolder holder) {
                    Debug.logI(TAG + "surfaceDestroyed");
                }
            });
        }
    }

    @Override
    protected void onNewIntent(Intent intent) {
        Debug.logI(TAG + "onNewIntent: intent=" + intent);
        super.onNewIntent(intent);
        this.setIntent(intent);
        this.mUnityPlayer.newIntent(intent);
    }

    @Override
    protected void onDestroy() {
        Debug.logI(TAG + "onDestroy");
        this.mUnityPlayer.destroy();
        super.onDestroy();
    }

    @Override
    protected void onStop() {
        Debug.logI(TAG + "onStop");
        super.onStop();
        this.mUnityPlayer.onStop();
    }

    @Override
    protected void onStart() {
        Debug.logI(TAG + "onStart");
        super.onStart();
        this.mUnityPlayer.onStart();
    }

    @Override
    protected void onPause() {
        Debug.logI(TAG + "onPause");
        super.onPause();
        this.mUnityPlayer.onPause();
    }

    @Override
    protected void onResume() {
        Debug.logI(TAG + "onResume");
        super.onResume();
        this.mUnityPlayer.onResume();
    }

    @Override
    public void onLowMemory() {
        Debug.logI(TAG + "onLowMemory");
        super.onLowMemory();
        this.mUnityPlayer.lowMemory();
    }

    @Override
    public void onTrimMemory(int level) {
        Debug.logI(TAG + "onTrimMemory: level=" + level);
        super.onTrimMemory(level);
        if (level == 15) {
            this.mUnityPlayer.lowMemory();
        }

    }

    @Override
    public void onConfigurationChanged(@NonNull Configuration newConfig) {
        Debug.logI(TAG + "onConfigurationChanged: newConfig=" + newConfig);
        super.onConfigurationChanged(newConfig);
        this.mUnityPlayer.configurationChanged(newConfig);
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        Debug.logI(TAG + "onWindowFocusChanged: hasFocus=" + hasFocus);
        super.onWindowFocusChanged(hasFocus);
        this.mUnityPlayer.windowFocusChanged(hasFocus);
    }

    @Override
    public boolean dispatchKeyEvent(KeyEvent event) {
//        Debug.logD(TAG + "dispatchKeyEvent: event=" + event);
        return event.getAction() == 2 ? this.mUnityPlayer.injectEvent(event) : super.dispatchKeyEvent(event);
    }

    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
//        Debug.logD(TAG + "onKeyUp: keyCode=" + keyCode + ", event=" + event);
        return this.mUnityPlayer.onKeyUp(keyCode, event);
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
//        Debug.logD(TAG + "onKeyUp: onKeyDown=" + keyCode + ", event=" + event);
        return this.mUnityPlayer.onKeyDown(keyCode, event);
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
//        Debug.logD(TAG + "onTouchEvent: event=" + event);
        return this.mUnityPlayer.onTouchEvent(event);
    }

    @Override
    public boolean onGenericMotionEvent(MotionEvent event) {
//        Debug.logD(TAG + "onGenericMotionEvent: event=" + event);
        return this.mUnityPlayer.onGenericMotionEvent(event);
    }

    private String updateUnityCommandLineArguments(String cmdLine) {
        return cmdLine;
    }

    private final IUnityPlayerLifecycleEvents mIUnityPlayerLifecycleEvents = new IUnityPlayerLifecycleEvents() {
        @Override
        public void onUnityPlayerUnloaded() {
            GameActivity.this.moveTaskToBack(true);
        }

        @Override
        public void onUnityPlayerQuitted() {

        }
    };
}

