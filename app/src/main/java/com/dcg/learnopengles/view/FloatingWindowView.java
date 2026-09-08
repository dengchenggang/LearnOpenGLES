package com.dcg.learnopengles.view;

import android.content.Context;
import android.graphics.PixelFormat;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.WindowManager;
import android.widget.FrameLayout;

import com.dcg.learnopengles.R;

public class FloatingWindowView extends FrameLayout {
    private final WindowManager mWindowManager;
    private OnCloseListener mOnCloseListener;

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

    public void attachToWindow() {
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
    }

    public void detachFromWindow() {
        mWindowManager.removeView(this);
    }
}
