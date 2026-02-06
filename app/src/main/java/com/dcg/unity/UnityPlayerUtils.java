package com.dcg.unity;

import android.view.SurfaceView;
import android.view.TextureView;
import android.view.View;
import android.widget.FrameLayout;

import androidx.annotation.NonNull;

import com.dcg.utils.Debug;

import java.util.ArrayList;
import java.util.List;

public class UnityPlayerUtils {

    private static final String TAG = "UnityPlayerUtils::";

    private static void findSurfaceView(@NonNull final FrameLayout unityPlayer, int i, final List<SurfaceView> surfaceViewList, final List<TextureView> textureViewList) {
        for (int j = 0; j < unityPlayer.getChildCount(); ++j) {
            final View child = unityPlayer.getChildAt(j);
            if (child instanceof SurfaceView) {
                Debug.logI(TAG, "findSurfaceView: child[%d, %d] is SurfaceView", i, j);
                if (surfaceViewList != null) {
                    surfaceViewList.add((SurfaceView) child);
                }
            } else if (child instanceof TextureView) {
                Debug.logI(TAG, "findSurfaceView: child[%d, %d] is TextureView", i, j);
                if (textureViewList != null) {
                    textureViewList.add((TextureView) child);
                }
            } else if (child instanceof FrameLayout) {
                UnityPlayerUtils.findSurfaceView((FrameLayout) child, j, surfaceViewList, textureViewList);
            } else {
                Debug.logI(TAG, "findSurfaceView: child[%d, %d] is %s", i, j, child.getClass());
            }
        }
    }

    private static void findSurfaceView(@NonNull final FrameLayout unityPlayer, final List<SurfaceView> surfaceViewList, final List<TextureView> textureViewList) {
        for (int i = 0; i < unityPlayer.getChildCount(); ++i) {
            final View child = unityPlayer.getChildAt(i);
            if (child instanceof SurfaceView) {
                Debug.logI(TAG, "findSurfaceView: child[%d] is SurfaceView", i);
                if (surfaceViewList != null) {
                    surfaceViewList.add((SurfaceView) child);
                }
            } else if (child instanceof TextureView) {
                Debug.logI(TAG, "findSurfaceView: child[%d] is TextureView", i);
                if (textureViewList != null) {
                    textureViewList.add((TextureView) child);
                }
            } else if (child instanceof FrameLayout) {
                UnityPlayerUtils.findSurfaceView((FrameLayout) child, i, surfaceViewList, textureViewList);
            } else {
                Debug.logI(TAG, "findSurfaceView: child[%d] is %s", i, child.getClass());
            }
        }
    }

    public static SurfaceView findSurfaceView(@NonNull final FrameLayout unityPlayer) {
        final List<SurfaceView> surfaceViewList = new ArrayList<>(1);
        UnityPlayerUtils.findSurfaceView(unityPlayer, surfaceViewList, null);
        Debug.logI(TAG + "findSurfaceView: surfaceViewList.size=" + surfaceViewList.size());
        return surfaceViewList.isEmpty() ? null : surfaceViewList.get(0);
    }
}
