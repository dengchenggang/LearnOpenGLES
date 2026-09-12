package com.dcg.learnopengles.view;

import android.content.Context;
import android.graphics.PixelFormat;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.FrameLayout;

import com.dcg.utils.Debug;
import com.dcg.learnopengles.R;

public class FloatingWindowView extends FrameLayout {
    private static final String TAG = "FloatingWindowView";
    private final WindowManager mWindowManager;
    private OnCloseListener mOnCloseListener;
    private boolean mWindowAdded;

    public interface OnCloseListener {
        void onClose();
    }

    public FloatingWindowView(Context context) {
        super(context);
        mWindowManager = (WindowManager) context.getSystemService(Context.WINDOW_SERVICE);
        LayoutInflater.from(context).inflate(R.layout.layout_floating_window, this, true);
        findViewById(R.id.btn_close_window).setOnClickListener(v -> {
            detachFromWindow();
            if (mOnCloseListener != null) {
                mOnCloseListener.onClose();
            }
        });
    }

    public void setOnCloseListener(OnCloseListener listener) {
        mOnCloseListener = listener;
    }

    public synchronized void attachToWindow() {
        if (mWindowAdded) {
            Debug.logI(TAG, "attachToWindow: already added");
            return;
        }
        WindowManager.LayoutParams params = new WindowManager.LayoutParams(
                WindowManager.LayoutParams.WRAP_CONTENT,
                WindowManager.LayoutParams.WRAP_CONTENT,
                WindowManager.LayoutParams.TYPE_APPLICATION_OVERLAY,
                WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE,
                PixelFormat.TRANSLUCENT);
        params.gravity = Gravity.TOP | Gravity.START;
        params.x = 0;
        params.y = 0;
        mWindowManager.addView(this, params);
        mWindowAdded = true;
        Debug.logI(TAG, "attachToWindow: added, size=%dx%d, position=(%d,%d)",
                getWidth(), getHeight(), params.x, params.y);
    }

    public synchronized void detachFromWindow() {
        if (mWindowAdded) {
            mWindowManager.removeView(this);
            mWindowAdded = false;
            Debug.logI(TAG, "detachFromWindow: removed");
        }
    }
}
