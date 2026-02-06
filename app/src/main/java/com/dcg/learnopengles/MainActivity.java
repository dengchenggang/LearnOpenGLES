package com.dcg.learnopengles;

import android.os.Bundle;

import com.dcg.learnopengles.databinding.ActivityMainBinding;
import com.unity3d.player.UnityPlayerActivity;

public class MainActivity extends UnityPlayerActivity {

    private ActivityMainBinding binding;
    private EGLSurfaceView mEGLSurfaceView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
//        mEGLSurfaceView = new EGLSurfaceView(this);
//        setContentView(mEGLSurfaceView);
    }
}